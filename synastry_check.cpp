#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include <cstring>
#include <cstdlib>
#include <map>

extern "C" {
#include "swephexp.h"
}

struct City {
    std::string name;
    double lat;
    double lon;
    double tz_offset;
};

const std::vector<City> city_db = {
    {"Nellore", 14.450000, 79.986944, 5.5},
    {"Kovur", 14.483333, 79.983333, 5.5},
    {"Hyderabad", 17.385044, 78.486671, 5.5},
    {"Chennai", 13.082680, 80.270718, 5.5},
    {"Bitragunta", 13.966700, 80.083300, 5.5}
};

struct Person {
    double jd_ut;
    double lat;
    double lon;
    std::string name;
};

struct Body {
    std::string name;
    int id;
    double lon;
    double lat = 0;
};

struct Chart {
    std::vector<Body> bodies;
    double houses[13] = {0};
    double ascmc[10] = {0};
    std::string name;
};

struct Hit {
    std::string name;
    std::string reason;
    std::string effect;
};

struct SoulmateResult {
    int score = 0;
    std::vector<Hit> hits;
};

const int BODIES[] = {
    SE_SUN, SE_MOON, SE_MERCURY, SE_VENUS, SE_MARS,
    SE_JUPITER, SE_SATURN, SE_URANUS, SE_NEPTUNE, SE_PLUTO,
    SE_TRUE_NODE, SE_CHIRON
};
const int AST_JUNO = SE_AST_OFFSET + 3;

// Add these enums/structs after your existing ones
enum Dignity {
    DIG_EXALTED, DIG_MOOLTRIKONA, DIG_OWN, DIG_FRIEND,
    DIG_NEUTRAL, DIG_ENEMY, DIG_DEBILITATED
};

struct Shadbala {
    double sthana_bala; // Positional strength
    double dig_bala; // Directional strength
    double kala_bala; // Temporal strength
    double chesta_bala; // Motional strength
    double naisargika_bala; // Natural strength
    double drik_bala; // Aspectual strength
    double total_bala;
    std::string summary;
};

struct CharaKaraka {
    int planet_id;
    std::string name;
    std::string karakatwa; // Atma, Amatya, Bhratri, etc
    double degree;
};

// Exaltation/Debilitation data per BPHS Ch.3
struct DignityData {
    int exalt_sign; double exalt_deg;
    int debil_sign; double debil_deg;
    int mooltrikona_sign; double mooltrikona_start; double mooltrikona_end;
    int own_signs[2]; // some planets own 2 signs
};

const DignityData DIGNITY_TABLE[10] = {
    // Sun: Aries 10° exalt, Libra 10° debil, Leo 0-20 MT, Leo own
    {0, 10.0, 6, 10.0, 4, 0.0, 20.0, {4, -1}},
    // Moon: Taurus 3° exalt, Scorpio 3° debil, Taurus 4-30 MT, Cancer own
    {1, 3.0, 7, 3.0, 1, 4.0, 30.0, {3, -1}},
    // Mars: Capricorn 28° exalt, Cancer 28° debil, Aries 0-12 MT, Aries/Scorpio own
    {9, 28.0, 3, 28.0, 0, 0.0, 12.0, {0, 7}},
    // Mercury: Virgo 15° exalt, Pisces 15° debil, Virgo 16-20 MT, Gemini/Virgo own
    {5, 15.0, 11, 15.0, 5, 16.0, 20.0, {2, 5}},
    // Jupiter: Cancer 5° exalt, Capricorn 5° debil, Sagittarius 0-10 MT, Sag/Pisces own
    {3, 5.0, 9, 5.0, 8, 0.0, 10.0, {8, 11}},
    // Venus: Pisces 27° exalt, Virgo 27° debil, Libra 0-15 MT, Taurus/Libra own
    {11, 27.0, 5, 27.0, 6, 0.0, 15.0, {1, 6}},
    // Saturn: Libra 20° exalt, Aries 20° debil, Aquarius 0-20 MT, Cap/Aqua own
    {6, 20.0, 0, 20.0, 10, 0.0, 20.0, {9, 10}},
    // Rahu: Taurus/Gemini exalt per some schools, Scorpio/Sag debil - disputed
    {1, 0.0, 7, 0.0, -1, 0.0, 0.0, {-1, -1}},
    // Ketu: Scorpio/Sag exalt, Taurus/Gemini debil - disputed
    {7, 0.0, 1, 0.0, -1, 0.0, 0.0, {-1, -1}},
    // Dummy for index 9
    {-1, 0.0, -1, 0.0, -1, 0.0, 0.0, {-1, -1}}
};

// Natural strength per BPHS Ch.27 - units of Shashtiamsa
const double NAISARGIKA_BALA[7] = {60.0, 51.43, 17.14, 25.71, 34.28, 42.85, 8.57};

Dignity get_dignity(int planet_id, double lon) {
    if (planet_id < SE_SUN || planet_id > SE_SATURN) return DIG_NEUTRAL;
    int idx = planet_id - SE_SUN;
    int sign = (int)(lon / 30.0);
    double deg_in_sign = lon - sign * 30.0;

    auto& d = DIGNITY_TABLE[idx];

    // Check exaltation - within 1° orb
    if (sign == d.exalt_sign && fabs(deg_in_sign - d.exalt_deg) <= 1.0)
        return DIG_EXALTED;

    // Check debilitation
    if (sign == d.debil_sign && fabs(deg_in_sign - d.debil_deg) <= 1.0)
        return DIG_DEBILITATED;

    // Check mooltrikona
    if (sign == d.mooltrikona_sign && deg_in_sign >= d.mooltrikona_start
        && deg_in_sign <= d.mooltrikona_end)
        return DIG_MOOLTRIKONA;

    // Check own sign
    if (sign == d.own_signs[0] || sign == d.own_signs[1])
        return DIG_OWN;

    // Friend/enemy - simplified: natural friends per BPHS
    // Full table needs 49 relationships. This is demo.
    return DIG_NEUTRAL;
}

// Combustion check - BPHS Ch.7
bool is_combust(int planet_id, double planet_lon, double sun_lon) {
    if (planet_id == SE_SUN || planet_id == SE_RAHU) return false;

    double dist = deg_distance(planet_lon, sun_lon);
    double combust_orb[7] = {0, 12, 17, 14, 11, 10, 15}; // Moon, Mars, Merc, Jup, Ven, Sat

    if (planet_id >= SE_MOON && planet_id <= SE_SATURN) {
        int idx = planet_id - SE_MOON;
        return dist <= combust_orb[idx];
    }
    return false;
}

// Graha Yuddha - Planetary war per BPHS Ch.18
// Returns winner planet_id, or -1 if no war, or -2 if both lose
int check_graha_yuddha(Body* p1, Body* p2) {
    if (!p1 ||!p2) return -1;
    if (p1->id == SE_SUN || p1->id == SE_MOON || p2->id == SE_SUN || p2->id == SE_MOON)
        return -1; // Luminaries don't fight

    if (deg_distance(p1->lon, p2->lon) > 1.0) return -1; // Must be within 1°

    // Winner = lower ecliptic latitude (closer to ecliptic), or if same, northern wins
    if (p1->lat < p2->lat) return p1->id;
    if (p2->lat < p1->lat) return p2->id;
    return (p1->lat >= 0)? p1->id : p2->id; // Northern declination wins
}

// Sthana Bala - positional strength, 1 of 6 Shadbala components
double calc_sthana_bala(Body* p, Chart& c) {
    if (!p) return 0;
    double sb = 0;

    // 1. Uccha Bala - exaltation strength: 60 at exact exalt, 0 at debil
    Dignity dig = get_dignity(p->id, p->lon);
    if (dig == DIG_EXALTED) sb += 60;
    else if (dig == DIG_MOOLTRIKONA) sb += 45;
    else if (dig == DIG_OWN) sb += 30;
    else if (dig == DIG_FRIEND) sb += 15;
    else if (dig == DIG_ENEMY) sb += 7.5;
    else if (dig == DIG_DEBILITATED) sb += 0;
    else sb += 15; // neutral

    // 2. Saptavargaja Bala - 7 divisional strengths. Simplified: only D1+D9
    int house = get_house(p->lon, c.houses);
    if (house == 1 || house == 4 || house == 7 || house == 10) sb += 60; // Kendra
    else if (house == 2 || house == 5 || house == 8 || house == 11) sb += 30; // Panaphara
    else sb += 15; // Apoklima

    // 3. Ojayugma Bala - odd/even sign strength
    int sign = (int)(p->lon / 30.0);
    bool odd_sign = (sign % 2 == 0); // Aries=0=odd
    if ((p->id == SE_SUN || p->id == SE_MARS || p->id == SE_JUPITER) && odd_sign) sb += 15;
    if ((p->id == SE_MOON || p->id == SE_VENUS) &&!odd_sign) sb += 15;

    // 4. Kendra Bala - already counted above

    // 5. Drekkana Bala - 1st drekkana male, 2nd female, 3rd male
    int drekkana = (int)((p->lon - sign * 30.0) / 10.0);
    if ((p->id == SE_SUN || p->id == SE_MARS || p->id == SE_JUPITER) && (drekkana == 0 || drekkana == 2)) sb += 15;
    if ((p->id == SE_MOON || p->id == SE_VENUS) && drekkana == 1) sb += 15;

    return sb; // Max ~ 300+ for sthana alone
}

// Dig Bala - directional strength per BPHS Ch.29
double calc_dig_bala(Body* p, Chart& c) {
    if (!p) return 0;
    double db = 0;
    int house = get_house(p->lon, c.houses);

    // Sun/Mars strong in 10th: 60 points at MC, 0 at IC
    if (p->id == SE_SUN || p->id == SE_MARS) {
        double dist_from_ic = deg_distance(p->lon, c.ascmc[1] + 180.0);
        db = 60.0 * (1.0 - dist_from_ic / 180.0);
    }
    // Jupiter/Mercury strong in 1st: 60 at Asc, 0 at Dsc
    else if (p->id == SE_JUPITER || p->id == SE_MERCURY) {
        double dist_from_dsc = deg_distance(p->lon, c.ascmc[0] + 180.0);
        db = 60.0 * (1.0 - dist_from_dsc / 180.0);
    }
    // Moon/Venus strong in 4th: 60 at IC, 0 at MC
    else if (p->id == SE_MOON || p->id == SE_VENUS) {
        double dist_from_mc = deg_distance(p->lon, c.ascmc[1]);
        db = 60.0 * (1.0 - dist_from_mc / 180.0);
    }
    // Saturn strong in 7th: 60 at Dsc, 0 at Asc
    else if (p->id == SE_SATURN) {
        double dist_from_asc = deg_distance(p->lon, c.ascmc[0]);
        db = 60.0 * (1.0 - dist_from_asc / 180.0);
    }

    return db < 0? 0 : db;
}

// Chara Karakas per Jaimini Sutras 1.1.10-18
std::vector<CharaKaraka> calc_chara_karakas(Chart& c) {
    std::vector<std::pair<int, double>> planets;
    for (auto& b : c.bodies) {
        if (b.id >= SE_SUN && b.id <= SE_SATURN || b.id == SE_TRUE_NODE) {
            double deg_in_sign = fmod(b.lon, 30.0);
            planets.push_back({b.id, deg_in_sign});
        }
    }
    // Rahu: use opposite point for degree calc
    for (auto& p : planets) {
        if (p.first == SE_TRUE_NODE) p.second = 30.0 - p.second;
    }

    std::sort(planets.begin(), planets.end(),
              [](auto& a, auto& b) { return a.second > b.second; });

    std::vector<CharaKaraka> karakas;
    const char* names[8] = {"Atma Karaka", "Amatya Karaka", "Bhratri Karaka",
                            "Matri Karaka", "Putra Karaka", "Gnati Karaka",
                            "Dara Karaka", "Upakheta"};

    for (size_t i = 0; i < planets.size() && i < 8; i++) {
        auto* b = find_body(c, planets[i].first);
        if (b) {
            karakas.push_back({b->id, b->name, names[i], planets[i].second});
        }
    }
    return karakas;
}

// Full Shadbala wrapper
Shadbala calc_shadbala(Body* p, Chart& c) {
    Shadbala sb = {0};
    if (!p) return sb;

    sb.sthana_bala = calc_sthana_bala(p, c);
    sb.dig_bala = calc_dig_bala(p, c);
    sb.naisargika_bala = (p->id >= SE_SUN && p->id <= SE_SATURN)?
                         NAISARGIKA_BALA[p->id - SE_SUN] : 0;

    // Kala, Chesta, Drik need ephemeris motion + aspects - skipping for brevity
    // You’d calculate those with swe_calc() at jd-1 and jd+1 for speed
    sb.kala_bala = 30; // placeholder
    sb.chesta_bala = 30; // placeholder
    sb.drik_bala = 0; // placeholder

    sb.total_bala = sb.sthana_bala + sb.dig_bala + sb.kala_bala +
                    sb.chesta_bala + sb.naisargika_bala + sb.drik_bala;

    // BPHS: >390 = strong, 300-390 = medium, <300 = weak
    if (sb.total_bala > 390) sb.summary = "Strong";
    else if (sb.total_bala > 300) sb.summary = "Medium";
    else sb.summary = "Weak";

    return sb;
}

// Demo: Print Shadbala for all planets
void print_shadbala_chart(Person per) {
    Chart c = calc_chart(per);
    printf("\n=== Shadbala Analysis: %s ===\n", per.name.c_str());
    printf("%-10s %8s %8s %8s %8s %8s %8s %8s\n",
           "Planet", "Sthana", "Dig", "Kala", "Chesta", "Naisarg", "Drik", "Total");

    for (auto& b : c.bodies) {
        if (b.id < SE_SUN || b.id > SE_SATURN) continue;
        Shadbala sb = calc_shadbala(&b, c);
        printf("%-10s %8.1f %8.1f %8.1f %8.1f %8.1f %8.1f %8.1f %s\n",
               b.name.c_str(), sb.sthana_bala, sb.dig_bala, sb.kala_bala,
               sb.chesta_bala, sb.naisargika_bala, sb.drik_bala, sb.total_bala,
               sb.summary.c_str());
    }

    printf("\nChara Karakas:\n");
    auto cks = calc_chara_karakas(c);
    for (auto& ck : cks) {
        printf(" %s: %s %.2f°\n", ck.karakatwa.c_str(), ck.name.c_str(), ck.degree);
    }
}

double angle_diff(double a, double b) {
    double d = fmod(a - b + 360.0, 360.0);
    if (d > 180.0) d -= 360.0;
    return d;
}

double deg_distance(double a, double b) {
    return fabs(angle_diff(a, b));
}

double midpoint(double a, double b) {
    double diff = angle_diff(b, a);
    return fmod(a + diff/2.0 + 360.0, 360.0);
}

bool has_aspect(double lon1, double lon2, double aspect, double orb) {
    return fabs(deg_distance(lon1, lon2) - aspect) <= orb;
}

bool has_any_aspect(double lon1, double lon2, const std::vector<double>& aspects, double orb) {
    for (double asp : aspects) if (has_aspect(lon1, lon2, asp, orb)) return true;
    return false;
}

Body* find_body(Chart& c, int id) {
    for (auto& b : c.bodies) if (b.id == id) return &b;
    return nullptr;
}

Chart calc_chart(Person p) {
    Chart c;
    c.name = p.name;
    double xx[6];
    char serr[256];
    char pname[64];

    for (int id : BODIES) {
        Body b;
        b.id = id;
        if (swe_calc_ut(p.jd_ut, id, SEFLG_SWIEPH | SEFLG_SIDEREAL, xx, serr) >= 0) {
            b.lon = xx[0];
            swe_get_planet_name(id, pname);
            b.name = pname;
            c.bodies.push_back(b);
        }
    }

    Body juno;
    juno.id = AST_JUNO;
    if (swe_calc_ut(p.jd_ut, AST_JUNO, SEFLG_SWIEPH | SEFLG_SIDEREAL, xx, serr) >= 0) {
        juno.lon = xx[0];
        juno.name = "Juno";
        c.bodies.push_back(juno);
    }

    swe_houses_ex(p.jd_ut, SEFLG_SWIEPH | SEFLG_SIDEREAL, p.lat, p.lon, 'P', c.houses, c.ascmc);

    Body vtx;
    vtx.id = -100;
    vtx.lon = c.ascmc[3];
    vtx.name = "Vertex";
    c.bodies.push_back(vtx);

    return c;
}

const char* zodiac_sign(double lon) {
    const char* signs[12] = {"Aries","Taurus","Gemini","Cancer","Leo","Virgo",
                             "Libra","Scorpio","Sagittarius","Capricorn","Aquarius","Pisces"};
    int sign = (int)(lon / 30.0) % 12;
    return signs[sign];
}

void print_natal_chart(Person p) {
    Chart c = calc_chart(p);
    printf("\n=== Natal Chart: %s ===\n", p.name.c_str());
    printf("Birth: %s\n", p.name.c_str());
    printf("Lat: %.4f, Lon: %.4f\n", p.lat, p.lon);
    printf("JD UT: %.6f\n\n", p.jd_ut);

    printf("Planets:\n");
    for (auto& b : c.bodies) {
        if (b.id == -100) continue;
        double lon = b.lon;
        int sign = (int)(lon / 30.0);
        double deg = lon - sign * 30.0;
        int d = (int)deg;
        int m = (int)((deg - d) * 60);
        int s = (int)((((deg - d) * 60) - m) * 60);

        printf("%-10s %2d°%02d'%02d\" %-12s %.6f°\n",
               b.name.c_str(), d, m, s, zodiac_sign(lon), lon);
    }

    printf("\nAngles:\n");
    printf("Ascendant %2d°%02d'%02d\" %-12s %.6f°\n",
           (int)fmod(c.ascmc[0],30), (int)(fmod(c.ascmc[0],1)*60),
           (int)(fmod(c.ascmc[0]*60,1)*60), zodiac_sign(c.ascmc[0]), c.ascmc[0]);
    printf("MC %2d°%02d'%02d\" %-12s %.6f°\n",
           (int)fmod(c.ascmc[1],30), (int)(fmod(c.ascmc[1],1)*60),
           (int)(fmod(c.ascmc[1]*60,1)*60), zodiac_sign(c.ascmc[1]), c.ascmc[1]);

    auto* vtx = find_body(c, -100);
    if (vtx) {
        printf("Vertex %2d°%02d'%02d\" %-12s %.6f°\n",
               (int)fmod(vtx->lon,30), (int)(fmod(vtx->lon,1)*60),
               (int)(fmod(vtx->lon*60,1)*60), zodiac_sign(vtx->lon), vtx->lon);
    }

    printf("\nHouses (Placidus):\n");
    for (int i = 1; i <= 12; i++) {
        printf("House %2d: %2d°%02d' %-12s %.6f°\n", i,
               (int)fmod(c.houses[i],30), (int)(fmod(c.houses[i],1)*60),
               zodiac_sign(c.houses[i]), c.houses[i]);
    }
}

int get_house(double lon, double houses[13]) {
    for (int i = 1; i <= 12; i++) {
        int next = i == 12? 1 : i + 1;
        double start = houses[i];
        double end = houses[next];
        if (end < start) end += 360.0;
        double test = lon;
        if (test < start) test += 360.0;
        if (test >= start && test < end) return i;
    }
    return 1;
}

Chart calc_composite(Chart& a, Chart& b) {
    Chart c;
    c.name = "Composite";
    c.bodies.reserve(a.bodies.size());

    for (size_t i = 0; i < a.bodies.size(); i++) {
        Body mb;
        mb.id = a.bodies[i].id;
        mb.name = a.bodies[i].name;
        mb.lon = midpoint(a.bodies[i].lon, b.bodies[i].lon);
        c.bodies.push_back(mb);
    }

    double armc = midpoint(a.ascmc[2], b.ascmc[2]);
    double houses[13] = {0};
    double ascmc[10] = {0};

    swe_houses_armc(armc, 0.0, 0.0, 'P', houses, ascmc);

    for (int i = 0; i < 13; i++) c.houses[i] = houses[i];
    for (int i = 0; i < 10; i++) c.ascmc[i] = ascmc[i];

    return c;
}

SoulmateResult check_soulmate_marriage(Chart& a, Chart& b) {
    SoulmateResult res;
    std::vector<double> soft = {0, 60, 120};
    std::vector<double> hard_soft = {0, 60, 120, 90, 180};

    auto* a_sun = find_body(a, SE_SUN);
    auto* b_sun = find_body(b, SE_SUN);
    auto* a_moon = find_body(a, SE_MOON);
    auto* b_moon = find_body(b, SE_MOON);
    auto* a_ven = find_body(a, SE_VENUS);
    auto* b_ven = find_body(b, SE_VENUS);
    auto* a_mars = find_body(a, SE_MARS);
    auto* b_mars = find_body(b, SE_MARS);
    auto* a_sat = find_body(a, SE_SATURN);
    auto* b_sat = find_body(b, SE_SATURN);
    auto* a_nn = find_body(a, SE_TRUE_NODE);
    auto* b_nn = find_body(b, SE_TRUE_NODE);
    auto* a_juno = find_body(a, AST_JUNO);
    auto* b_juno = find_body(b, AST_JUNO);
    auto* a_vtx = find_body(a, -100);
    auto* b_vtx = find_body(b, -100);

    if (!a_sun||!b_sun||!a_moon||!b_moon||!a_ven||!b_ven) return res;

    if (has_any_aspect(a_sun->lon, b_moon->lon, {0,180,120,60}, 5.0) ||
        has_any_aspect(b_sun->lon, a_moon->lon, {0,180,120,60}, 5.0)) {
        res.score++;
        res.hits.push_back({
            "Sun-Moon interaspect",
            "Core polarity: Sun = conscious ego/masculine, Moon = emotional needs/feminine. Contact shows yin-yang balance.",
            "Natural understanding. You 'get' each other's core self and emotional reactions. Strong marriage glue."
        });
    }

    if (a_mars && b_mars && (has_any_aspect(a_ven->lon, b_mars->lon, hard_soft, 4.0) ||
        has_any_aspect(b_ven->lon, a_mars->lon, hard_soft, 4.0))) {
        res.score++;
        res.hits.push_back({
            "Venus-Mars interaspect",
            "Venus = attraction/love, Mars = desire/sexual drive. Contact sparks romantic chemistry.",
            "Physical magnetism and passion. Keeps romance alive long-term. Without it, can feel platonic."
        });
    }

    auto check_node = [&](Body* node, Chart& c) {
        if (!node) return false;
        double sn_lon = fmod(node->lon + 180.0, 360.0);
        auto* c_sun = find_body(c, SE_SUN);
        auto* c_moon = find_body(c, SE_MOON);
        auto* c_ven = find_body(c, SE_VENUS);
        return deg_distance(node->lon, c_sun->lon) <= 3.0 || deg_distance(sn_lon, c_sun->lon) <= 3.0 ||
               deg_distance(node->lon, c_moon->lon) <= 3.0 || deg_distance(sn_lon, c_moon->lon) <= 3.0 ||
               deg_distance(node->lon, c_ven->lon) <= 3.0 || deg_distance(sn_lon, c_ven->lon) <= 3.0 ||
               deg_distance(node->lon, c.ascmc[0]) <= 3.0 || deg_distance(sn_lon, c.ascmc[0]) <= 3.0;
    };
    if (check_node(a_nn, b) || check_node(b_nn, a)) {
        res.score++;
        res.hits.push_back({
            "Node to personal planet/ASC",
            "North/South Node = karmic axis. Contact to Sun/Moon/Venus/ASC shows past-life tie or fated purpose.",
            "Instant recognition, 'I've known you forever' feeling. Relationship feels destined and teaches major soul lessons."
        });
    }

    auto vtx_hit = [&](Body* vtx, Chart& c) {
        if (!vtx) return false;
        auto* c_sun = find_body(c, SE_SUN);
        auto* c_moon = find_body(c, SE_MOON);
        auto* c_ven = find_body(c, SE_VENUS);
        auto* c_mars = find_body(c, SE_MARS);
        return deg_distance(vtx->lon, c_sun->lon) <= 2.0 ||
               deg_distance(vtx->lon, c_moon->lon) <= 2.0 ||
               deg_distance(vtx->lon, c_ven->lon) <= 2.0 ||
               (c_mars && deg_distance(vtx->lon, c_mars->lon) <= 2.0) ||
               deg_distance(vtx->lon, c.ascmc[0]) <= 2.0;
    };
    if (vtx_hit(a_vtx, b) || vtx_hit(b_vtx, a) || (a_vtx && b_vtx && deg_distance(a_vtx->lon, b_vtx->lon) <= 2.0)) {
        res.score++;
        res.hits.push_back({
            "Vertex activation",
            "Vertex = point of fated encounters. When activated, meetings feel orchestrated by universe.",
            "Synchronistic timing. You meet when both are ready for transformation. Often 'can't avoid each other' energy."
        });
    }

    auto in_7th = [&](Body* p, double h7, double h8) {
        if (!p) return false;
        if (h8 < h7) h8 += 360.0;
        double test = p->lon;
        if (test < h7) test += 360.0;
        return test >= h7 && test < h8;
    };
    bool h7_hit = in_7th(b_sun, a.houses[7], a.houses[8]) || in_7th(b_moon, a.houses[7], a.houses[8]) ||
                  in_7th(b_ven, a.houses[7], a.houses[8]) || in_7th(b_mars, a.houses[7], a.houses[8]) ||
                  in_7th(a_sun, b.houses[7], b.houses[8]) || in_7th(a_moon, b.houses[7], b.houses[8]) ||
                  in_7th(a_ven, b.houses[7], b.houses[8]) || in_7th(a_mars, b.houses[7], b.houses[8]);
    if (h7_hit) {
        res.score++;
        res.hits.push_back({
            "7th house overlay",
            "7th house = marriage/partnership house. Planets falling here means you embody 'spouse' role for each other.",
            "Natural projection of marriage onto each other. You literally see them as husband/wife material, not just dating."
        });
    }

    auto sat_harmony = [&](Body* sat, Chart& c) {
        if (!sat) return false;
        auto* c_sun = find_body(c, SE_SUN);
        auto* c_moon = find_body(c, SE_MOON);
        auto* c_ven = find_body(c, SE_VENUS);
        return has_any_aspect(sat->lon, c_sun->lon, soft, 4.0) ||
               has_any_aspect(sat->lon, c_moon->lon, soft, 4.0) ||
               has_any_aspect(sat->lon, c_ven->lon, soft, 4.0);
    };
    if (sat_harmony(a_sat, b) || sat_harmony(b_sat, a)) {
        res.score++;
        res.hits.push_back({
            "Saturn harmony to Sun/Moon/Venus",
            "Saturn = commitment, longevity, duty. Harmonious aspects = stabilizing, not restricting.",
            "Staying power. You can handle each other's bad days. Relationship ages well and survives real-life stress."
        });
    }

    Chart comp = calc_composite(a, b);
    auto* c_sun = find_body(comp, SE_SUN);
    auto* c_moon = find_body(comp, SE_MOON);
    auto* c_ven = find_body(comp, SE_VENUS);
    auto* c_sat = find_body(comp, SE_SATURN);
    if (c_sun && c_moon && c_ven) {
        int moon_house = get_house(c_moon->lon, comp.houses);
        int ven_house = get_house(c_ven->lon, comp.houses);
        bool comp_hit = (deg_distance(c_sun->lon, comp.ascmc[0]) <= 3.0 || deg_distance(c_sun->lon, comp.ascmc[1]) <= 3.0) ||
                        (moon_house == 7 || moon_house == 4 || ven_house == 7 || ven_house == 4) ||
                        (c_sat && has_any_aspect(c_ven->lon, c_sat->lon, soft, 3.0));
        if (comp_hit) {
            res.score++;
            res.hits.push_back({
                "Strong composite Sun/Moon/Venus",
                "Composite = chart of relationship itself. Sun/Moon/Venus on angles or 4th/7th shows marriage identity.",
                "The relationship has its own life as a 'marriage entity'. Feels like a real unit, not just two people dating."
            });
        }
    }

    auto juno_hit = [&](Body* juno, Chart& c) {
        if (!juno) return false;
        auto* c_sun = find_body(c, SE_SUN);
        auto* c_moon = find_body(c, SE_MOON);
        return deg_distance(juno->lon, c_sun->lon) <= 2.0 ||
               deg_distance(juno->lon, c_moon->lon) <= 2.0 ||
               deg_distance(juno->lon, c.ascmc[0]) <= 2.0;
    };
    if (juno_hit(a_juno, b) || juno_hit(b_juno, a) || (a_juno && b_juno && deg_distance(a_juno->lon, b_juno->lon) <= 2.0)) {
        res.score++;
        res.hits.push_back({
            "Juno marriage contacts",
            "Juno = asteroid of committed partnership and legal marriage. Rules fidelity, contracts, 'spouse energy'.",
            "Marriage-minded bond. Triggers desire for exclusivity, formal commitment, and playing traditional spouse roles."
        });
    }

    return res;
}

const City* find_city(const std::string& name) {
    for (const auto& c : city_db) {
        if (strcasecmp(c.name.c_str(), name.c_str()) == 0) return &c;
    }
    return nullptr;
}

struct TransitWindow {
    int start_y, start_m, start_d;
    int end_y, end_m, end_d;
    std::string planet;
    std::string target;
    std::string meaning;
};

std::vector<TransitWindow> scan_timing(Person p, int start_y, int start_m, int end_y, int end_m) {
    std::vector<TransitWindow> windows;
    std::map<std::string, TransitWindow*> active;

    Chart natal = calc_chart(p);
    double dsc = natal.houses[7];
    auto* natal_ven = find_body(natal, SE_VENUS);
    auto* natal_juno = find_body(natal, AST_JUNO);
    auto* natal_vtx = find_body(natal, -100);

    double jd_start = swe_julday(start_y, start_m, 1, 0, SE_GREG_CAL);
    double jd_end = swe_julday(end_y, end_m, 28, 23.999, SE_GREG_CAL);
    char serr[256];
    double xx[6];

    auto check_and_add = [&](double jday, double lon, double natal_pt, const char* pl, const char* tgt, const char* mean) {
        if (deg_distance(lon, natal_pt) >= 1.0) return;

        int y, m, d; double h;
        swe_revjul(jday, SE_GREG_CAL, &y, &m, &d, &h);
        std::string key = std::string(pl) + "->" + tgt;

        if (active.count(key)) {
            active[key]->end_y = y;
            active[key]->end_m = m;
            active[key]->end_d = d;
        } else {
            windows.push_back({y,m,d,y,m,d,pl,tgt,mean});
            active[key] = &windows.back();
        }
    };

    for (double jday = jd_start; jday <= jd_end; jday += 1.0) {
        swe_calc_ut(jday, SE_JUPITER, SEFLG_SWIEPH | SEFLG_SIDEREAL, xx, serr);
        double jup = xx[0];
        check_and_add(jday, jup, dsc, "Jupiter", "DSC", "Meeting/expansion window");
        if (natal_ven) check_and_add(jday, jup, natal_ven->lon, "Jupiter", "Venus", "Love/romance opportunity");
        if (natal_juno) check_and_add(jday, jup, natal_juno->lon, "Jupiter", "Juno", "Marriage/commitment trigger");
        if (natal_vtx) check_and_add(jday, jup, natal_vtx->lon, "Jupiter", "Vertex", "Fated meeting");

        swe_calc_ut(jday, SE_SATURN, SEFLG_SWIEPH | SEFLG_SIDEREAL, xx, serr);
        double sat = xx[0];
        check_and_add(jday, sat, dsc, "Saturn", "DSC", "Karmic partner, commitment test");
        if (natal_ven) check_and_add(jday, sat, natal_ven->lon, "Saturn", "Venus", "Karmic commitment, serious love");

        swe_calc_ut(jday, SE_TRUE_NODE, SEFLG_SWIEPH | SEFLG_SIDEREAL, xx, serr);
        double nn = xx[0];
        if (natal_ven) check_and_add(jday, nn, natal_ven->lon, "Node", "Venus", "Fated love/karmic tie");
        check_and_add(jday, nn, dsc, "Node", "DSC", "Destiny brings partner");

        swe_calc_ut(jday, SE_URANUS, SEFLG_SWIEPH | SEFLG_SIDEREAL, xx, serr);
        double ura = xx[0];
        check_and_add(jday, ura, dsc, "Uranus", "DSC", "Sudden meeting, electric attraction");

        swe_calc_ut(jday, SE_PLUTO, SEFLG_SWIEPH | SEFLG_SIDEREAL, xx, serr);
        double plu = xx[0];
        if (natal_ven) check_and_add(jday, plu, natal_ven->lon, "Pluto", "Venus", "Obsessive/fated love, deep bond");
    }
    active.clear();
    return windows;
}

void print_compare(Person p1, Person p2, int sy, int sm, int ey, int em) {
    std::cout << "\n=== Coincident Timing Windows: " << p1.name << " + " << p2.name << " ===\n";
    std::cout << "Range: " << sy << "-" << sm << " to " << ey << "-" << em << "\n\n";

    auto w1 = scan_timing(p1, sy, sm, ey, em);
    auto w2 = scan_timing(p2, sy, sm, ey, em);

    struct Overlap {
        int start_y, start_m, start_d;
        int end_y, end_m, end_d;
        std::string a_desc, b_desc;
    };
    std::vector<Overlap> overlaps;

    for (auto& a : w1) {
        double a_start = swe_julday(a.start_y, a.start_m, a.start_d, 0, SE_GREG_CAL);
        double a_end = swe_julday(a.end_y, a.end_m, a.end_d, 23.999, SE_GREG_CAL);
        for (auto& b : w2) {
            double b_start = swe_julday(b.start_y, b.start_m, b.start_d, 0, SE_GREG_CAL);
            double b_end = swe_julday(b.end_y, b.end_m, b.end_d, 23.999, SE_GREG_CAL);

            if (a_start <= b_end && b_start <= a_end) {
                double ov_start = std::max(a_start, b_start);
                double ov_end = std::min(a_end, b_end);
                int y1,m1,d1,y2,m2,d2; double h;
                swe_revjul(ov_start, SE_GREG_CAL, &y1, &m1, &d1, &h);
                swe_revjul(ov_end, SE_GREG_CAL, &y2, &m2, &d2, &h);

                std::string a_desc = "T-" + a.planet + " to " + a.target;
                std::string b_desc = "T-" + b.planet + " to " + b.target;
                overlaps.push_back({y1,m1,d1,y2,m2,d2,a_desc,b_desc});
            }
        }
    }

    std::sort(overlaps.begin(), overlaps.end(), [](const Overlap& x, const Overlap& y) {
        if (x.start_y!= y.start_y) return x.start_y < y.start_y;
        if (x.start_m!= y.start_m) return x.start_m < y.start_m;
        return x.start_d < y.start_d;
    });

    int count = 0;
    for (size_t i = 0; i < overlaps.size(); i++) {
        if (i > 0 && overlaps[i].start_y == overlaps[i-1].start_y &&
            overlaps[i].start_m == overlaps[i-1].start_m &&
            overlaps[i].start_d == overlaps[i-1].start_d) continue;

        printf("%04d-%02d-%02d to %04d-%02d-%02d: %s | %s\n",
            overlaps[i].start_y, overlaps[i].start_m, overlaps[i].start_d,
            overlaps[i].end_y, overlaps[i].end_m, overlaps[i].end_d,
            overlaps[i].a_desc.c_str(), overlaps[i].b_desc.c_str());
        count++;
    }

    if (count == 0) std::cout << "No overlapping windows found in range.\n";
    else std::cout << "\nTotal distinct coincident windows: " << count << "\n";
}

// Add after your existing structs
struct AyushResult {
    std::string strongest; // "Lagna", "Sun", "Moon"
    double base_years;
    double final_years;
    std::vector<std::string> reductions;
    std::vector<std::string> additions;
    std::string category; // Alpayu, Madhyayu, Dirghayu
};

struct MarakaDasha {
    int planet_id;
    std::string planet_name;
    std::string reason; // "7th lord", "2nd lord", "in 7th"
};

// Helper: check if planet is benefic
bool is_benefic(int id) {
    return id == SE_JUPITER || id == SE_VENUS ||
           id == SE_MERCURY || id == SE_MOON;
}

// Helper: check if planet is malefic
bool is_malefic(int id) {
    return id == SE_SATURN || id == SE_MARS ||
           id == SE_SUN || id == SE_TRUE_NODE; // Rahu
}

// Get sign lord by Vedic rulership
int get_sign_lord(int sign) {
    int lords[12] = {SE_MARS, SE_VENUS, SE_MERCURY, SE_MOON, SE_SUN, SE_MERCURY,
                     SE_VENUS, SE_MARS, SE_JUPITER, SE_SATURN, SE_SATURN, SE_JUPITER};
    return lords[sign % 12];
}

// Simplified Shadbala: just positional + digbala for demo
// Real Shadbala needs 6 types of strength - 300+ lines
double simple_strength(Body* b, Chart& c) {
    if (!b) return 0;
    double strength = 0;
    int sign = (int)(b->lon / 30.0);
    int house = get_house(b->lon, c.houses);

    // Own sign/exaltation = strong
    int lord = get_sign_lord(sign);
    if (lord == b->id) strength += 60;

    // Kendra = strong
    if (house == 1 || house == 4 || house == 7 || house == 10) strength += 30;

    // Digbala: Sun/Mars 10th, Moon/Venus 4th, Merc/Jup 1st, Sat 7th
    if ((b->id == SE_SUN || b->id == SE_MARS) && house == 10) strength += 20;
    if ((b->id == SE_MOON || b->id == SE_VENUS) && house == 4) strength += 20;
    if ((b->id == SE_MERCURY || b->id == SE_JUPITER) && house == 1) strength += 20;
    if (b->id == SE_SATURN && house == 7) strength += 20;

    return strength;
}

// Main Lagna Ayurdaya per BPHS Ch.43
AyushResult calc_ayurdaya(Chart& c) {
    AyushResult res;

    double lagna = c.ascmc[0];
    auto* sun = find_body(c, SE_SUN);
    auto* moon = find_body(c, SE_MOON);

    // Step 1: Find strongest of Lagna, Sun, Moon
    double lagna_str = 50; // Base strength for lagna
    double sun_str = sun? simple_strength(sun, c) : 0;
    double moon_str = moon? simple_strength(moon, c) : 0;

    double base_lon;
    if (lagna_str >= sun_str && lagna_str >= moon_str) {
        res.strongest = "Lagna";
        base_lon = lagna;
    } else if (sun_str >= moon_str) {
        res.strongest = "Sun";
        base_lon = sun->lon;
    } else {
        res.strongest = "Moon";
        base_lon = moon->lon;
    }

    // Step 2: Base years = degrees traversed in sign
    int sign = (int)(base_lon / 30.0);
    res.base_years = base_lon - sign * 30.0;
    res.final_years = res.base_years;

    // Step 3: Additions - benefics in kendra/11th from base point
    for (auto& b : c.bodies) {
        if (b.id < SE_SUN || b.id > SE_SATURN) continue;
        int house_from_base = get_house(b.lon, c.houses) - get_house(base_lon, c.houses) + 1;
        if (house_from_base <= 0) house_from_base += 12;

        if (is_benefic(b.id) && (house_from_base == 1 || house_from_base == 4 ||
            house_from_base == 7 || house_from_base == 10 || house_from_base == 11)) {
            res.final_years += 1.0;
            res.additions.push_back(b.name + " benefic in kendra/11th");
        }
    }

    // Step 4: Reductions per BPHS - simplified set
    auto* lagna_lord = find_body(c, get_sign_lord((int)(lagna / 30.0)));
    auto* eighth_lord = find_body(c, get_sign_lord(((int)(lagna / 30.0) + 7) % 12));

    // Check if 8th lord in 6/8/12 - reduce 1/3
    if (eighth_lord) {
        int h = get_house(eighth_lord->lon, c.houses);
        if (h == 6 || h == 8 || h == 12) {
            res.final_years *= 0.666667;
            res.reductions.push_back("8th lord in 6/8/12: -1/3");
        }
    }

    // Malefics in 6/8/12 from base - reduce 0.5 year each
    for (auto& b : c.bodies) {
        if (!is_malefic(b.id) || b.id > SE_SATURN) continue;
        int house_from_base = get_house(b.lon, c.houses) - get_house(base_lon, c.houses) + 1;
        if (house_from_base <= 0) house_from_base += 12;
        if (house_from_base == 6 || house_from_base == 8 || house_from_base == 12) {
            res.final_years -= 0.5;
            res.reductions.push_back(b.name + " malefic in 6/8/12: -0.5yr");
        }
    }

    // Saturn in Lagna adds years
    auto* sat = find_body(c, SE_SATURN);
    if (sat && get_house(sat->lon, c.houses) == 1) {
        res.final_years += 5.0;
        res.additions.push_back("Saturn in Lagna: +5yr");
    }

    if (res.final_years < 0) res.final_years = 0;

    // Step 5: Classify
    if (res.final_years < 32) res.category = "Alpayu (<32)";
    else if (res.final_years < 70) res.category = "Madhyayu (32-70)";
    else res.category = "Dirghayu (70+)";

    return res;
}

// Find maraka planets per BPHS
std::vector<MarakaDasha> find_marakas(Chart& c) {
    std::vector<MarakaDasha> marakas;
    int lagna_sign = (int)(c.ascmc[0] / 30.0);

    // 2nd and 7th lords
    int second_lord = get_sign_lord((lagna_sign + 1) % 12);
    int seventh_lord = get_sign_lord((lagna_sign + 6) % 12);

    auto* sl = find_body(c, second_lord);
    auto* svl = find_body(c, seventh_lord);

    if (sl) marakas.push_back({sl->id, sl->name, "2nd lord"});
    if (svl) marakas.push_back({svl->id, svl->name, "7th lord"});

    // Planets in 2nd or 7th house
    for (auto& b : c.bodies) {
        if (b.id < SE_SUN || b.id > SE_SATURN) continue;
        int h = get_house(b.lon, c.houses);
        if (h == 2 || h == 7) {
            marakas.push_back({b.id, b.name, "in " + std::to_string(h) + "th"});
        }
    }
    return marakas;
}

// Print function - add to main()
void print_ayurdaya(Person p) {
    Chart c = calc_chart(p);
    printf("\n=== Ayurdaya Analysis: %s ===\n", p.name.c_str());
    printf("Method: Lagna Ayurdaya per Brihat Parashara Hora Shastra\n");
    printf("DISCLAIMER: For educational study only. Not medical advice.\n\n");

    AyushResult ar = calc_ayurdaya(c);

    printf("Strongest point: %s\n", ar.strongest.c_str());
    printf("Base years: %.2f\n", ar.base_years);

    if (!ar.additions.empty()) {
        printf("\nAdditions:\n");
        for (auto& a : ar.additions) printf(" + %s\n", a.c_str());
    }

    if (!ar.reductions.empty()) {
        printf("\nReductions:\n");
        for (auto& r : ar.reductions) printf(" - %s\n", r.c_str());
    }

    printf("\nCalculated years: %.2f\n", ar.final_years);
    printf("Category: %s\n", ar.category.c_str());

    auto marakas = find_marakas(c);
    printf("\nMaraka planets (health-sensitive in their dasha periods):\n");
    for (auto& m : marakas) {
        printf(" %s - %s\n", m.planet_name.c_str(), m.reason.c_str());
    }
    printf("\nNote: Actual longevity depends on karma, lifestyle, medical care.\n");
    printf("This shows sensitive dasha periods for health checkups only.\n");
}

int main(int argc, char** argv) {
    swe_set_ephe_path("./ephe");
    swe_set_sid_mode(SE_SIDM_LAHIRI, 0, 0);

    
	// Mode 4: Ayurdaya
	if (argc == 9 && std::string(argv[1]) == "--ayush") {
		const City* c = find_city(argv[8]);
		if (!c) { std::cerr << "City not found\n"; return 1; }
		double h = std::atof(argv[5]) + std::atof(argv[6])/60.0 + std::atof(argv[7])/3600.0 - c->tz_offset;
		Person p;
		p.jd_ut = swe_julday(std::atoi(argv[2]), std::atoi(argv[3]), std::atoi(argv[4]), h, SE_GREG_CAL);
		p.lat = c->lat; p.lon = c->lon; p.name = c->name;
		print_ayurdaya(p);
		swe_close();
		return 0;
	}
	// Mode 0: Natal chart dump
    if (argc == 9 && std::string(argv[1]) == "--natal") {
        const City* c = find_city(argv[8]);
        if (!c) { std::cerr << "City not found\n"; return 1; }
        double h = std::atof(argv[5]) + std::atof(argv[6])/60.0 + std::atof(argv[7])/3600.0 - c->tz_offset;
        Person p;
        p.jd_ut = swe_julday(std::atoi(argv[2]), std::atoi(argv[3]), std::atoi(argv[4]), h, SE_GREG_CAL);
        p.lat = c->lat; p.lon = c->lon; p.name = c->name;
        print_natal_chart(p);
        swe_close();
        return 0;
    }

    // Mode 1: Timing for 1 person with year range
    if (argc == 11 && std::string(argv[1]) == "--timing") {
        const City* c = find_city(argv[8]);
        if (!c) { std::cerr << "City not found\n"; return 1; }
        double h = std::atof(argv[5]) + std::atof(argv[6])/60.0 + std::atof(argv[7])/3600.0 - c->tz_offset;
        Person p;
        p.jd_ut = swe_julday(std::atoi(argv[2]), std::atoi(argv[3]), std::atoi(argv[4]), h, SE_GREG_CAL);
        p.lat = c->lat; p.lon = c->lon; p.name = c->name;
        int sy = std::atoi(argv[9]);
        int ey = std::atoi(argv[10]);
        auto wins = scan_timing(p, sy, 1, ey, 12);
        std::cout << "=== Timing Windows for " << p.name << " " << sy << "-01 to " << ey << "-12 ===\n";
        for (auto& w : wins) {
            printf("%04d-%02d-%02d to %04d-%02d-%02d: T-%s to %s - %s\n",
                w.start_y, w.start_m, w.start_d,
                w.end_y, w.end_m, w.end_d,
                w.planet.c_str(), w.target.c_str(), w.meaning.c_str());
        }
        if (wins.empty()) std::cout << "No major 1° hits in range.\n";
        swe_close();
        return 0;
    }

    // Mode 2: Compare both charts for coincident dates
    if (argc == 20 && std::string(argv[1]) == "--compare") {
        const City* c1 = find_city(argv[8]);
        const City* c2 = find_city(argv[15]);
        if (!c1 ||!c2) {
            std::cerr << "City not found. Looking for '" << argv[8] << "' and '" << argv[15] << "'\n";
            std::cerr << "Available: ";
            for (const auto& ct : city_db) std::cerr << ct.name << " ";
            std::cerr << "\n";
            return 1;
        }
        double h1 = std::atof(argv[5]) + std::atof(argv[6])/60.0 + std::atof(argv[7])/3600.0 - c1->tz_offset;
        double h2 = std::atof(argv[12]) + std::atof(argv[13])/60.0 + std::atof(argv[14])/3600.0 - c2->tz_offset;
        Person p1;
        p1.jd_ut = swe_julday(std::atoi(argv[2]), std::atoi(argv[3]), std::atoi(argv[4]), h1, SE_GREG_CAL);
        p1.lat = c1->lat; p1.lon = c1->lon; p1.name = c1->name;
        Person p2;
        p2.jd_ut = swe_julday(std::atoi(argv[9]), std::atoi(argv[10]), std::atoi(argv[11]), h2, SE_GREG_CAL);
        p2.lat = c2->lat; p2.lon = c2->lon; p2.name = c2->name;
        int sy = std::atoi(argv[16]), sm = std::atoi(argv[17]);
        int ey = std::atoi(argv[18]), em = std::atoi(argv[19]);
        print_compare(p1, p2, sy, sm, ey, em);
        swe_close();
        return 0;
    }

    // Mode 3: Original synastry
    if (argc == 15) {
        const City* c1 = find_city(argv[7]);
        const City* c2 = find_city(argv[14]);
        if (!c1 ||!c2) {
            std::cerr << "Error: City not found.\nAvailable:";
            for (const auto& ct : city_db) std::cerr << " " << ct.name;
            std::cerr << "\n";
            return 1;
        }
        // Fixed: argv[4]=hour, [5]=min, [6]=sec
        double h1 = std::atof(argv[4]) + std::atof(argv[5])/60.0 + std::atof(argv[6])/3600.0 - c1->tz_offset;
        double h2 = std::atof(argv[11]) + std::atof(argv[12])/60.0 + std::atof(argv[13])/3600.0 - c2->tz_offset;
        Person p1; p1.jd_ut = swe_julday(std::atoi(argv[1]), std::atoi(argv[2]), std::atoi(argv[3]), h1, SE_GREG_CAL);
        p1.lat = c1->lat; p1.lon = c1->lon; p1.name = c1->name;
        Person p2; p2.jd_ut = swe_julday(std::atoi(argv[8]), std::atoi(argv[9]), std::atoi(argv[10]), h2, SE_GREG_CAL);
        p2.lat = c2->lat; p2.lon = c2->lon; p2.name = c2->name;
        Chart ch1 = calc_chart(p1);
        Chart ch2 = calc_chart(p2);

        auto* vtx1 = find_body(ch1, -100);
        auto* vtx2 = find_body(ch2, -100);

        if (vtx1 && vtx2) {
            printf("\n=== Vertex Synastry Contacts ===\n");
            for (auto& b : ch2.bodies) {
                if (b.id >= SE_SUN && b.id <= SE_PLUTO || b.id == AST_JUNO || b.id == SE_CHIRON) {
                    double dist = deg_distance(b.lon, vtx1->lon);
                    if (dist <= 2.0) {
                        printf("P2 %s conjunct P1 Vertex: %.2f° orb - Fated contact\n", b.name.c_str(), dist);
                    }
                }
            }
            for (auto& b : ch1.bodies) {
                if (b.id >= SE_SUN && b.id <= SE_PLUTO || b.id == AST_JUNO || b.id == SE_CHIRON) {
                    double dist = deg_distance(b.lon, vtx2->lon);
                    if (dist <= 2.0) {
                        printf("P1 %s conjunct P2 Vertex: %.2f° orb - Fated contact\n", b.name.c_str(), dist);
                    }
                }
            }
        }

        SoulmateResult res = check_soulmate_marriage(ch1, ch2);
        std::cout << "=== Soulmate Marriage Signature ===\n";
        std::cout << p1.name << " + " << p2.name << "\n";
        std::cout << "Score: " << res.score << "/8\n\n";
        for (const auto& h : res.hits) {
            std::cout << "[+] " << h.name << "\n";
            std::cout << " Reason: " << h.reason << "\n";
            std::cout << " Effect: " << h.effect << "\n\n";
        }
        if (res.score >= 6) std::cout << "Result: HIGH PROBABILITY signature\n";
        else if (res.score >= 4) std::cout << "Result: Moderate signature\n";
        else std::cout << "Result: Low signature\n";
        swe_close();
        return 0;
    }

    std::cerr << "Usage:\n";
    std::cerr << " Natal:./synastry --natal Y M D H Mi S City\n";
    std::cerr << " Synastry:./synastry Y1 M1 D1 H1 Mi1 S1 City1 Y2 M2 D2 H2 Mi2 S2 City2\n";
    std::cerr << " Timing:./synastry --timing Y M D H Mi S City StartYear EndYear\n";
    std::cerr << " Compare:./synastry --compare Y1...City1 Y2...City2 StartY StartM EndY EndM\n";
    return 1;
}