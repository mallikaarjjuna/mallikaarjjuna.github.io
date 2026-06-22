#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <algorithm>
#include <ctime>
#include <map>
#include "json.hpp"      // <--- ADD THIS

extern "C" {
    #include "swephexp.h"
}

#include "shadbala.h" 
#include "texts.h"       // <--- NEW PHASE 4: EXTERNALIZED TEXTS
#include "texts_te.h"    // <--- TELUGU TRANSLATION ARRAYS

using namespace std;
using json = nlohmann::json; // <--- ADD THIS

// =========================================================================
// DATA STRUCTURES & CONSTANTS
// =========================================================================

struct City { string name; double lat; double lon; double tz_offset; };

const vector<City> city_db = {
    {"Nellore", 14.450000, 79.986944, 5.5},
    {"Kovur", 14.483333, 79.983333, 5.5},
    {"Hyderabad", 17.385044, 78.486671, 5.5},
    {"Chennai", 13.082680, 80.270718, 5.5},
    {"Bitragunta", 13.966700, 80.083300, 5.5},
    {"Krishna", 16.270300, 80.996700, 5.5},
    // --- MAJOR METROPOLITAN ADDITIONS FOR WEB UI ---
    {"Bangalore", 12.971600, 77.594600, 5.5},
    {"Mumbai", 19.076000, 72.877700, 5.5},
    {"Delhi", 28.613900, 77.209000, 5.5},
    {"Kolkata", 22.572600, 88.363900, 5.5}
};

const char* en_short_p_names[] = {"Asc", "Su", "Mo", "Ma", "Me", "Ju", "Ve", "Sa", "Ra", "Ke"};

struct Transition { int rashi_idx, nak_idx, pada, h, m, s; bool is_rashi_change; };
struct Karaka { int p_idx; double deg; };
struct DashaPeriod { int lord_idx; double start_jd; double duration; };
struct DashaState { double start; double duration; int lord; };
struct TransitHit { string p_name; string hit_type; };

// =========================================================================
// HELP MENU DEFINITION
// =========================================================================

void print_help_menu() {
    printf("\n=== JYOTISHA ENGINE HELP MENU ===\n");
    printf("Usage: ./astroc <YYYY> <MM> <DD> <HH> <MIN> <SEC> <CityName> [Command] [Args...]\n\n");
    printf("Commands (Optional):\n");
    printf("  all                            : Displays Natal Chart, Aspects, Vargas, AV, Panchang, and Dasha.\n");
    printf("  analyze <Dx>                   : Runs AI-style interpretations for D1 or any varga.\n");
    printf("  dasha                          : Launches interactive menu to drill down all 6 Dasha layers.\n");
    printf("  dasha all                      : Exports the entire 120-year 6-level hierarchy to CSV.\n");
    printf("  dasha YYYY MM DD [HH:MM:SS]    : Calculates the 6-layer Dasha active on target date.\n");
    printf("  daily                          : Sweeps the birth date for daily transitions and Horas.\n");
    printf("  daily YYYY MM DD               : Sweeps the specified target date for daily transitions.\n");
    printf("  deha                           : Calculates Deha Dashas intersecting the current system date.\n");
    printf("  deha YYYY MM DD [HH:MM:SS]     : Pinpoints Deha Dasha for the exact target date/time.\n");
    printf("  transit                        : Calculates current Transits (Gochar) vs Natal Moon and Ascendant.\n");
    printf("  transit YYYY MM DD [HH:MM:SS]  : Calculates Natal Cross-Aspects for a specific target date/time.\n");
    printf("  collision <planet> <YYYY> [MM] [DD] : Finds exact transit collisions over natal planet degree (+/- 2 deg).\n");
	printf("  predict <StartY> <EndY>        : Scans timeframe for exact Jup/Sat/Sun/Ven/Dasha alignments (Single Chart).\n");
    printf("  match <Y> <M> <D> <H> <m> <s> <City> : Runs the Universal V8.4 Synastry Audit against Person 2.\n");
    printf("  match_predict <StartY> <EndY> <Y> <M> <D> <H> <m> <s> <City> : Scans timeframe for MUTUAL intersection dates.\n\n");
    printf("  kp                             : Calculates Krishnamurti Paddhati (KP) tables.\n");
    printf("  json                           : Silences all tables and outputs purely formatted JSON.\n\n");
}

// =========================================================================
// ENGINE CLASS
// =========================================================================

class JyotishaEngine {
public:
    bool json_mode = false;
	bool telugu_mode = false;
	string get_month_name(int month) const {
        const char* te_months[] = {"", "జనవరి", "ఫిబ్రవరి", "మార్చి", "ఏప్రిల్", "మే", "జూన్", "జూలై", "ఆగస్టు", "సెప్టెంబరు", "అక్టోబరు", "నవంబరు", "డిసెంబరు"};
        return telugu_mode ? te_months[month] : to_string(month);
    }
    
    string get_paksha(int tithi_idx) const {
        return telugu_mode ? ((tithi_idx < 15) ? "శుక్ల పక్షం" : "కృష్ణ పక్షం") : ((tithi_idx < 15) ? "Shukla" : "Krishna");
    }
	// ==========================================
    // TELUGU TRANSLATION WRAPPERS
    // ==========================================
    string get_planet_name(int idx) const { return telugu_mode ? te_p_names_full[idx] : p_names_full[idx]; }
// Change your get_short_planet wrapper to this:
	string get_short_planet(int idx) const { return telugu_mode ? te_short_p_names[idx] : en_short_p_names[idx]; }
    string get_rashi_name(int idx) const { return telugu_mode ? te_rashi_names[idx] : rashi_names[idx]; }
    string get_short_rashi(int idx) const { return telugu_mode ? te_short_rashi[idx] : short_rashi[idx]; }
    string get_nak_name(int idx) const { return telugu_mode ? te_nak_names[idx] : nak_names[idx]; }
    string get_weekday(int idx) const { return telugu_mode ? te_weekdays[idx] : weekdays[idx]; }
    string get_tithi(int idx) const { return telugu_mode ? te_tithi_names[idx] : tithi_names[idx]; }
    string get_yoga(int idx) const { return telugu_mode ? te_yoga_names[idx] : yoga_names[idx]; }
    string get_dasha_lord(int idx) const { return telugu_mode ? te_dasha_lords[idx] : dasha_lords[idx]; }
    string get_dasha_level(int idx) const { return telugu_mode ? te_dasha_levels[idx] : dasha_levels[idx]; }
    string get_tara(int idx) const { return telugu_mode ? te_tara_names[idx] : tara_names[idx]; }
    // ==========================================

    double tjd_ut;
    int32 iflag;
    City location;
    
    double local_hour_decimal;
    double sunrise_hour_decimal = 6.0, sunset_hour_decimal = 18.0;
    int current_weekday = 0;
    
    double sun_lon, moon_lon, lagna_lon;
    double planet_lons[10];
	int natal_scores[10] = {0}; // Global storage for Auspiciousness scores	
    double house_cusps[13]; 
    string rashi_grid[12] = {"", "", "", "", "", "", "", "", "", "", "", ""};
    int planet_rashis[10];
	
	// --- New Phase 4 Jaimini Variables ---
    int atmakaraka_idx = 0;
    int darakaraka_idx = 0;

    int get_arudha(int target_house_num) const {
        int lagna = planet_rashis[0];
        int start_sign = (lagna + target_house_num - 1) % 12;
        string lord_name = rashi_lords[start_sign];
        int lord_idx = 1;
        for (int p=1; p<=7; p++) { if (string(p_names_full[p]) == lord_name) lord_idx = p; }
        int lord_sign = planet_rashis[lord_idx];
        int steps = (lord_sign - start_sign + 12) % 12;
        int arudha_sign = (lord_sign + steps) % 12;
        // Jaimini Exceptions: If lord is in its own sign or 7th from it
        if (steps == 0) arudha_sign = (arudha_sign + 9) % 12; // 10th therefrom
        else if (steps == 6) arudha_sign = (arudha_sign + 3) % 12; // 4th therefrom
        return arudha_sign;
    }
	
// --- New Phase 3 Variables ---
    int sav_scores[12] = {0}; 
    int bav_scores[7][12] = {0};
    bool av_calculated = false;	
    
    string json_output = "{\n";

    // Change this line:
    JyotishaEngine(int y, int m, int d, int h, int min, int sec, City loc, bool j_mode, bool t_mode = false) {
        location = loc; json_mode = j_mode; telugu_mode = t_mode;
        local_hour_decimal = h + (min / 60.0) + (sec / 3600.0);
        double time_ut = local_hour_decimal - location.tz_offset;
        swe_set_ephe_path((char*)"./ephe"); 
        swe_set_sid_mode(SE_SIDM_LAHIRI, 0, 0);
        tjd_ut = swe_julday(y, m, d, time_ut, SE_GREG_CAL);
        swe_set_topo(location.lon, location.lat, 0.0);
        iflag = SEFLG_SWIEPH | SEFLG_SIDEREAL | SEFLG_SPEED | SEFLG_TRUEPOS;

        if (!json_mode) {
            printf("\n=== PURE ASTRONOMICAL C++ ENGINE OUTPUT ===\n");
            printf("Local Date: %02d/%02d/%04d | Local Time: %02d:%02d:%02d\n", d, m, y, h, min, sec);
            printf("Location: %s (Lat: %f, Lon: %f, TZ: %+.1f)\n", location.name.c_str(), location.lat, location.lon, location.tz_offset);
        }
    }

    ~JyotishaEngine() { swe_close(); }

    string format_time_only(double jd) {
        int y, m, d; double jut;
        swe_revjul(jd + (location.tz_offset / 24.0), SE_GREG_CAL, &y, &m, &d, &jut);
        int h = (int)jut; int min = (int)((jut - h) * 60.0); int sec = (int)round((((jut - h) * 60.0) - min) * 60.0);
        if (sec >= 60) { sec -= 60; min += 1; } if (min >= 60) { min -= 60; h += 1; }
        char buf[32]; snprintf(buf, sizeof(buf), "%02d:%02d:%02d", h, min, sec);
        return string(buf);
    }

    string jd_to_string(double jd) {
        int y, m, d; double jut;
        swe_revjul(jd + (location.tz_offset / 24.0), SE_GREG_CAL, &y, &m, &d, &jut);
        int h = (int)jut; int min = (int)((jut - h) * 60.0); int sec = (int)round((((jut - h) * 60.0) - min) * 60.0);
        if (sec >= 60) { sec -= 60; min += 1; } if (min >= 60) { min -= 60; h += 1; }
        char buf[64]; snprintf(buf, sizeof(buf), "%02d/%02d/%04d %02d:%02d:%02d", d, m, y, h, min, sec);
        return string(buf);
    }

    string format_dms(double decimal_degrees) {
        int d = (int)decimal_degrees; double f_deg = decimal_degrees - d;
        int m = (int)(f_deg * 60.0); int s = (int)round((f_deg * 60.0 - m) * 60.0);
        if (s >= 60) { s -= 60; m += 1; } if (m >= 60) { m -= 60; d += 1; }
        int sign = d / 30; d = d % 30;
        char buf[32]; snprintf(buf, sizeof(buf), "%02d° %s %02d'%02d\"", d, short_rashi[sign], m, s);
        return string(buf);
    }

    double get_planet_lon_on_jd(int p_idx, double jd) {
        int planets[] = {SE_SUN, SE_MOON, SE_MARS, SE_MERCURY, SE_JUPITER, SE_VENUS, SE_SATURN, SE_TRUE_NODE};
        double xx[6]; char serr[256];
        if (p_idx == 0) {
            double cusps[13], ascmc[10];
            swe_houses_ex(jd, iflag, location.lat, location.lon, 'P', cusps, ascmc);
            return ascmc[0];
        } else if (p_idx < 9) {
            swe_calc_ut(jd, planets[p_idx-1], iflag, xx, serr);
            return xx[0];
        } else { 
            swe_calc_ut(jd, SE_TRUE_NODE, iflag, xx, serr);
            return fmod(xx[0] + 180.0, 360.0);
        }
    }

	int get_varga(int v_num, double lon) const {
		int rashi = (int)(lon / 30.0); double deg = lon - (rashi * 30.0);
        int p, s = 0, m, b, e;
        switch(v_num) {
            case 1: s = rashi; break;
            case 2: if(rashi%2==0) s=(deg<15)?4:3; else s=(deg<15)?3:4; break;
            case 3: p=(int)(deg/10.0); s=(rashi+p*4)%12; break;
            case 4: p=(int)(deg/7.5); s=(rashi+p*3)%12; break;
            case 7: p=(int)(deg/(30.0/7.0)); s=(rashi%2==0)?(rashi+p)%12:(rashi+6+p)%12; break;
            case 9: p=(int)(lon/(10.0/3.0)); s=p%12; break;
            case 10: p=(int)(deg/3.0); s=(rashi%2==0)?(rashi+p)%12:(rashi+8+p)%12; break;
            case 11: p=(int)(deg/(30.0/11.0)); b=(12-rashi)%12; s=(b+p)%12; break;
            case 12: p=(int)(deg/2.5); s=(rashi+p)%12; break;
            case 16: p=(int)(deg/(30.0/16.0)); m=rashi%3; b=(m==0)?0:(m==1?4:8); s=(b+p)%12; break;
            case 20: p=(int)(deg/1.5); m=rashi%3; b=(m==0)?0:(m==1?8:4); s=(b+p)%12; break;
            case 24: p=(int)(deg/1.25); b=(rashi%2==0)?4:3; s=(b+p)%12; break;
            case 27: p=(int)(deg/(30.0/27.0)); e=rashi%4; b=(e==0)?0:(e==1?3:(e==2?6:9)); s=(b+p)%12; break;
            case 30: if(rashi%2==0){if(deg<5)s=0;else if(deg<10)s=10;else if(deg<18)s=8;else if(deg<25)s=2;else s=6;}
                     else{if(deg<5)s=1;else if(deg<12)s=5;else if(deg<20)s=11;else if(deg<25)s=9;else s=7;} break;
            case 40: p=(int)(deg/0.75); b=(rashi%2==0)?0:6; s=(b+p)%12; break;
            case 45: p=(int)(deg/(30.0/45.0)); m=rashi%3; b=(m==0)?0:(m==1?4:8); s=(b+p)%12; break;
            case 60: p=(int)(deg/0.5); s=(rashi+p)%12; break;
            default: s = rashi; break;
        } return s;
    }

	void calculate_chart() {
        double ascmc[10];
        if (swe_houses_ex(tjd_ut, iflag, location.lat, location.lon, 'P', house_cusps, ascmc) >= 0) {
            lagna_lon = ascmc[0]; planet_lons[0] = lagna_lon; planet_rashis[0] = (int)(lagna_lon / 30.0); 
            if (!json_mode) {
                printf("--------------------------------------------------------------------------------------------------------------------------\n");
                // Dynamic Telugu/English Headers
                if (telugu_mode) {
                    printf("%-15s | %-19s | %-10s | %-25s | %-30s | %-10s | %-10s\n", "గ్రహం", "రేఖాంశం", "D9 రాశి", "నక్షత్రం (పాదం)", "తార (నవతార)", "న. అధిపతి", "రా. అధిపతి");
                } else {
                    printf("%-15s | %-19s | %-10s | %-25s | %-30s | %-10s | %-10s\n", "Graha", "Longitude", "D9 Rasi", "Nakshatra (Pada)", "Tara (Navatara)", "N. Lord", "R. Lord");
                }
                printf("--------------------------------------------------------------------------------------------------------------------------\n");
            }
            // Passing Integer Index 0 for Lagna
            process_planet(0, lagna_lon); 
        }

        int planets[] = {SE_SUN, SE_MOON, SE_MARS, SE_MERCURY, SE_JUPITER, SE_VENUS, SE_SATURN, SE_TRUE_NODE};
        double xx[6]; char serr[256];

        for (int i = 0; i < 8; i++) {
            if (swe_calc_ut(tjd_ut, planets[i], iflag, xx, serr) >= 0) {
                planet_lons[i+1] = xx[0]; planet_rashis[i+1] = (int)(xx[0] / 30.0);
                // Passing Integer Indices 1 through 8
                process_planet(i+1, xx[0]); 
                if (planets[i] == SE_SUN) sun_lon = xx[0]; if (planets[i] == SE_MOON) moon_lon = xx[0]; 
            }
        }
        swe_calc_ut(tjd_ut, SE_TRUE_NODE, iflag, xx, serr);
        planet_lons[9] = fmod(xx[0] + 180.0, 360.0); planet_rashis[9] = (int)(planet_lons[9] / 30.0);
        // Passing Integer Index 9 for Ketu
        process_planet(9, planet_lons[9]);

        if (!json_mode) {
            printf("--------------------------------------------------------------------------------------------------------------------------\n");
            draw_south_indian_chart();
        }
		// --- JAIMINI KARAKAS (Soul Significators) ---
        if (!json_mode) {
            if (telugu_mode) printf("\n[జైమిని కారకత్వాలు & ఆరూఢ లగ్నాలు]\n");
            else printf("\n[JAIMINI KARAKAS & ARUDHAS]\n");
        }
        struct Karaka { int p_idx; double deg; };
        std::vector<Karaka> karakas;
        
        for (int i = 1; i <= 7; i++) karakas.push_back({i, fmod(planet_lons[i], 30.0)});
        std::sort(karakas.begin(), karakas.end(), [](const Karaka& a, const Karaka& b) { return a.deg > b.deg; });

        atmakaraka_idx = karakas[0].p_idx;
        darakaraka_idx = karakas[6].p_idx;

        if (!json_mode) {
            const char* k_names_en[] = { "Atmakaraka (AK)   [Soul]", "Amatyakaraka (AmK) [Career]", "Bhratrukaraka (BK) [Siblings]", "Matrukaraka (MK)   [Mother]", "Pitrukaraka (PiK)  [Father]", "Putrakaraka (PuK)  [Children]", "Darakaraka (DK)    [Spouse]" };
            const char* k_names_te[] = { "ఆత్మకారక (AK)   [ఆత్మ]", "అమాత్యకారక (AmK) [కెరీర్]", "భ్రాతృకారక (BK) [సోదరులు]", "మాతృకారక (MK)   [తల్లి]", "పితృకారక (PiK)  [తండ్రి]", "పుత్రకారక (PuK)  [సంతానం]", "దారకారక (DK)    [భాగస్వామి]" };
        
            for (int i = 0; i < 7; i++) {
                int deg = (int)karakas[i].deg;
                int min = (int)((karakas[i].deg - deg) * 60.0);
                if (telugu_mode) printf("%-28s : %-10s (%02d° %02d')\n", k_names_te[i], get_planet_name(karakas[i].p_idx).c_str(), deg, min);
                else printf("%-26s : %-10s (%02d° %02d')\n", k_names_en[i], p_names_full[karakas[i].p_idx], deg, min);
            }
        }
        
        // --- UPA PADA LAGNA (UL) CALCULATION ---
        int h12_rashi = (planet_rashis[0] + 11) % 12; 
        int l12_idx = 1; 
        for(int x = 1; x <= 7; x++) { if(string(rashi_lords[h12_rashi]) == p_names_full[x]) l12_idx = x; }
        int lord_rashi = planet_rashis[l12_idx];
        int distance = (lord_rashi - h12_rashi + 12) % 12;
        int ul_rashi = (lord_rashi + distance) % 12;
        if (ul_rashi == h12_rashi || ul_rashi == (h12_rashi + 6) % 12) ul_rashi = (ul_rashi + 9) % 12; 
        
        if (!json_mode) {
            if (telugu_mode) printf("%-28s : %-10s\n", "ఉపపద లగ్నం (UL) [వివాహం]", get_rashi_name(ul_rashi).c_str());
            else printf("%-26s : %-10s\n", "Upa Pada Lagna (UL)", rashi_names[ul_rashi]);
            printf("-----------------------------------------------------------------\n");
        }
    }
	
    void process_planet(int p_idx, double decimal_degrees) {
        int rashi_index = (int)(decimal_degrees / 30.0); double rashi_degrees = decimal_degrees - (rashi_index * 30.0);
        int degrees = (int)rashi_degrees; int minutes = (int)((rashi_degrees - degrees) * 60.0);
        int seconds = (int)round((((rashi_degrees - degrees) * 60.0) - minutes) * 60.0);
        if (seconds >= 60) { seconds -= 60; minutes += 1; } if (minutes >= 60) { minutes -= 60; degrees += 1; }
        if (degrees >= 30) { degrees -= 30; rashi_index = (rashi_index + 1) % 12; }

        // Added wrapper for the South Indian Rasi Chart grid
        rashi_grid[rashi_index] += get_short_planet(p_idx) + " ";
        
        int d9_rashi_index = ((int)(decimal_degrees / (10.0 / 3.0))) % 12;
        double nak_size = 360.0 / 27.0; int nak_index = (int)(decimal_degrees / nak_size);
        int pada = (int)((decimal_degrees - (nak_index * nak_size)) / (nak_size / 4.0)) + 1;
        int nak_lord_index = nak_index % 9;
        int natal_mo_nak = (int)(moon_lon / (360.0 / 27.0)); int tara_idx = (nak_index - natal_mo_nak + 27) % 9;
        
        // Added wrappers for Nakshatra names
        string nak_pada = get_nak_name(nak_index) + " " + to_string(pada);
        
        // Find Rashi Lord Index (1-7) to translate the Lord's Name
        int r_lord_idx = 1;
        for (int p = 1; p <= 7; p++) { if (string(p_names_full[p]) == rashi_lords[rashi_index]) r_lord_idx = p; }
        
        if (!json_mode) {
            // Replaced hardcoded English arrays with the Telugu-aware wrappers
            printf("%-15s | %02d° %-10s %02d' %02d\" | %-10s | %-25s | %-30s | %-10s | %-10s\n", 
                   get_planet_name(p_idx).c_str(), degrees, get_rashi_name(rashi_index).c_str(), minutes, seconds, 
                   get_rashi_name(d9_rashi_index).c_str(), nak_pada.c_str(), get_tara(tara_idx).c_str(), 
                   get_dasha_lord(nak_lord_index).c_str(), get_planet_name(r_lord_idx).c_str());
        }
    }
	
    void draw_south_indian_chart() {
        if (telugu_mode) {
            printf("\n=== రాశి చక్రం (దక్షిణ భారత శైలి) ===\n");
            printf("-----------------------------------------------------------------\n");
            for (int i = 0; i < 12; i++) {
                printf("%2d. %-15s : %s\n", i+1, get_rashi_name(i).c_str(), rashi_grid[i].c_str());
            }
            printf("-----------------------------------------------------------------\n");
        } else {
            auto c = [&](int idx) { string s = rashi_grid[idx]; if (s.length() > 14) s = s.substr(0, 14); return s; };
            printf("\n=== SOUTH INDIAN RASI CHART ===\n");
            printf("-----------------------------------------------------------------\n");
            printf("| %-14s| %-14s| %-14s| %-14s|\n", "12 Meen", "1 Mesh", "2 Vrish", "3 Mitu");
            printf("| %-14s| %-14s| %-14s| %-14s|\n", c(11).c_str(), c(0).c_str(), c(1).c_str(), c(2).c_str());
            printf("|               |               |               |               |\n");
            printf("-----------------------------------------------------------------\n");
            printf("| %-14s|                               | %-14s|\n", "11 Kumb", "4 Kark");
            printf("| %-14s|                               | %-14s|\n", c(10).c_str(), c(3).c_str());
            printf("|               |                               |               |\n");
            printf("-----------------           RASI CHART          -----------------\n");
            printf("| %-14s|                               | %-14s|\n", "10 Maka", "5 Simh");
            printf("| %-14s|                               | %-14s|\n", c(9).c_str(), c(4).c_str());
            printf("|               |                               |               |\n");
            printf("-----------------------------------------------------------------\n");
            printf("| %-14s| %-14s| %-14s| %-14s|\n", "9 Dhan", "8 Vrishch", "7 Tula", "6 Kany");
            printf("| %-14s| %-14s| %-14s| %-14s|\n", c(8).c_str(), c(7).c_str(), c(6).c_str(), c(5).c_str());
            printf("|               |               |               |               |\n");
            printf("-----------------------------------------------------------------\n");
        }
    }
	
// =========================================================================
    // COLLISION SWEEPER (UPGRADED: UNIFIED BUBBLE & "ALL" FEATURE)
    // =========================================================================

    double get_dist(double p1, double p2) {
        double d = abs(p1 - p2);
        if (d > 180.0) d = 360.0 - d;
        return d;
    }

    // Unified helper function to guarantee 0-minute discrepancy
    void refine_bubble(int p_idx, double target_lon, double approx_jd, double orb, double &e_in, double &e_peak, double &e_out) {
        // 1. Find exact entry (walk backwards until out of orb)
        double cur = approx_jd;
        while(get_dist(get_planet_lon_on_jd(p_idx, cur), target_lon) <= orb) { cur -= (1.0/1440.0); }
        e_in = cur + (1.0/1440.0);
        
        // 2. Find exact exit (walk forwards until out of orb)
        cur = approx_jd;
        while(get_dist(get_planet_lon_on_jd(p_idx, cur), target_lon) <= orb) { cur += (1.0/1440.0); }
        e_out = cur - (1.0/1440.0);
        
        // 3. Find exact peak between e_in and e_out
        e_peak = e_in;
        double min_d = 999.0;
        for (double d = e_in; d <= e_out; d += (1.0/1440.0)) {
            double dist = get_dist(get_planet_lon_on_jd(p_idx, d), target_lon);
            if (dist < min_d) { min_d = dist; e_peak = d; }
        }
    }

	void sweep_collisions(int p_idx, double start_jd, double end_jd) {
		double target_lon = planet_lons[p_idx];
		int natal_rashi = (int)(target_lon / 30.0); // Sign Gatekeeper
		const double orb = 2.0;
		
		printf("\n=== TRANSIT COLLISION SWEEP: %s (Sign: %s) ===\n", p_names_full[p_idx], rashi_names[natal_rashi]);
		printf("Target Natal Degree: %s\n", format_dms(target_lon).c_str());
		
		if (p_idx == 0) {
			printf("Search Window: %s to %s\n", jd_to_string(start_jd).c_str(), jd_to_string(end_jd).c_str());
		}
		printf("-----------------------------------------------------------------------------------------------------------------\n");

		double step = 4.0 / 24.0; 
		if (p_idx == 0) step = 5.0 / 1440.0; 
		else if (p_idx == 2) step = 1.0 / 24.0; 
		
		int hit_count = 0;
		double scan_start = start_jd - 30.0;
		double scan_end = end_jd + 30.0;
		if (p_idx == 0) { scan_start = start_jd - 1.0; scan_end = end_jd + 1.0; }
		if (p_idx == 2) { scan_start = start_jd - 5.0; scan_end = end_jd + 5.0; }

		for (double jd = scan_start; jd <= scan_end; jd += step) {
			double trans_lon = get_planet_lon_on_jd(p_idx, jd);
			int trans_rashi = (int)(trans_lon / 30.0); // Check Transit Sign
			
			// --- STRICT RASHI GATE ---
			if (trans_rashi != natal_rashi) continue; 
			
			double dist = get_dist(trans_lon, target_lon);

			if (dist <= orb) {
				double e_in, e_peak, e_out;
				refine_bubble(p_idx, target_lon, jd, orb, e_in, e_peak, e_out);

				if (e_peak >= start_jd && e_peak <= end_jd) {
					printf("Hit %d:  ENTER => %-20s | EXACT PEAK => %-20s | EXIT => %-20s\n", 
						   ++hit_count, jd_to_string(e_in).c_str(), jd_to_string(e_peak).c_str(), jd_to_string(e_out).c_str());
				}
				jd = e_out; // Fast-forward
			}
		}

		// Fallback Search
		if (hit_count == 0) {
			printf("No exact peak collisions occurred within the requested timeframe.\n");
			printf("Scanning to find the True Past and Future peaks...\n\n");
			
			double search_limit = 30.0 * 365.25;
			if (p_idx == 0) search_limit = 2.0;
			if (p_idx == 2) search_limit = 40.0;
			
			// Search Past
			double search_jd = start_jd; bool found_past = false;
			while (search_jd >= start_jd - search_limit) { 
				search_jd -= step; 
				double trans_lon = get_planet_lon_on_jd(p_idx, search_jd);
				if ((int)(trans_lon / 30.0) == natal_rashi && get_dist(trans_lon, target_lon) <= orb) {
					double e_in, e_peak, e_out;
					refine_bubble(p_idx, target_lon, search_jd, orb, e_in, e_peak, e_out);
					printf("Closest Past Hit   : ENTER => %-20s | EXACT PEAK => %-20s | EXIT => %-20s\n", 
						   jd_to_string(e_in).c_str(), jd_to_string(e_peak).c_str(), jd_to_string(e_out).c_str());
					found_past = true; break;
				}
			}
			if (!found_past) printf("No past hit found within limit.\n");

			// Search Future
			search_jd = end_jd; bool found_future = false;
			while (search_jd <= end_jd + search_limit) { 
				search_jd += step;
				double trans_lon = get_planet_lon_on_jd(p_idx, search_jd);
				if ((int)(trans_lon / 30.0) == natal_rashi && get_dist(trans_lon, target_lon) <= orb) {
					double e_in, e_peak, e_out;
					refine_bubble(p_idx, target_lon, search_jd, orb, e_in, e_peak, e_out);
					printf("Closest Future Hit : ENTER => %-20s | EXACT PEAK => %-20s | EXIT => %-20s\n", 
						   jd_to_string(e_in).c_str(), jd_to_string(e_peak).c_str(), jd_to_string(e_out).c_str());
					found_future = true; break;
				}
			}
		}
		printf("-----------------------------------------------------------------------------------------------------------------\n");
	}
    void calculate_collisions(string p_name, int t_year, int t_month, int t_day, bool is_year_only, bool is_month_only) {
        string p_lower = p_name;
        transform(p_lower.begin(), p_lower.end(), p_lower.begin(), ::tolower);
        
        vector<int> targets;
        
        if (p_lower == "all") {
            for (int i = 0; i <= 9; i++) targets.push_back(i);
        } else {
            int p_idx = -1;
            if (p_lower == "lagna" || p_lower == "ascendant" || p_lower == "asc") p_idx = 0;
            else if (p_lower == "surya" || p_lower == "sun" || p_lower == "ravi") p_idx = 1;
            else if (p_lower == "chandra" || p_lower == "moon") p_idx = 2;
            else if (p_lower == "mangal" || p_lower == "mars" || p_lower == "kuja") p_idx = 3;
            else if (p_lower == "budha" || p_lower == "mercury") p_idx = 4;
            else if (p_lower == "guru" || p_lower == "jupiter" || p_lower == "brihaspati") p_idx = 5;
            else if (p_lower == "shukra" || p_lower == "venus" || p_lower == "sukra") p_idx = 6;
            else if (p_lower == "shani" || p_lower == "saturn") p_idx = 7;
            else if (p_lower == "rahu") p_idx = 8;
            else if (p_lower == "ketu") p_idx = 9;
            
            if (p_idx == -1) { printf("Error: Planet '%s' not recognized.\n", p_name.c_str()); return; }
            targets.push_back(p_idx);
        }

        double start_jd, end_jd;
        if (is_year_only) {
            start_jd = swe_julday(t_year, 1, 1, 0.0 - location.tz_offset, SE_GREG_CAL);
            end_jd = swe_julday(t_year+1, 1, 1, 0.0 - location.tz_offset, SE_GREG_CAL);
        } else if (is_month_only) {
            start_jd = swe_julday(t_year, t_month, 1, 0.0 - location.tz_offset, SE_GREG_CAL);
            int next_m = t_month == 12 ? 1 : t_month + 1;
            int next_y = t_month == 12 ? t_year + 1 : t_year;
            end_jd = swe_julday(next_y, next_m, 1, 0.0 - location.tz_offset, SE_GREG_CAL);
        } else {
            start_jd = swe_julday(t_year, t_month, t_day, 0.0 - location.tz_offset, SE_GREG_CAL);
            end_jd = start_jd + 1.0;
        }

        if (p_lower == "all") {
            printf("\n=== GLOBAL COLLISION SWEEP ===\n");
            printf("Search Window: %s to %s\n", jd_to_string(start_jd).c_str(), jd_to_string(end_jd).c_str());
        }

        for (int p_idx : targets) {
            sweep_collisions(p_idx, start_jd, end_jd);
        }
    }
	
	// =========================================================================
    // PHASE 1: INTERPRETATION ENGINE (D1 OUTCOMES + VARGA FATE)
    // =========================================================================

	void analyze_chart(string varga_str) {
        int v_num = 1;
        if (varga_str.length() > 1 && varga_str[0] == 'D') v_num = stoi(varga_str.substr(1));

        int v_lagn_rasi = get_varga(v_num, planet_lons[0]);
        int v_planets[10];
        for(int i=0; i<10; i++) v_planets[i] = get_varga(v_num, planet_lons[i]);

        printf("\n================================================================================\n");
        printf("=== %s CHART ANALYSIS (Lagna: %s) ===\n", varga_str.c_str(), rashi_names[v_lagn_rasi]);
        printf("================================================================================\n");

        if (v_num == 1) {
            analyze_functional_nature(v_lagn_rasi);
            analyze_yogas(v_planets, v_lagn_rasi);
            analyze_doshas(v_planets, v_lagn_rasi);
            analyze_placements(v_planets, v_lagn_rasi);
            analyze_lordships(v_lagn_rasi, v_planets);
            analyze_auspiciousness(v_lagn_rasi, v_planets);			
            analyze_conjunctions(v_planets, v_lagn_rasi);
            analyze_final_outcomes(v_lagn_rasi, v_planets);
        } else {
            analyze_varga_synthesis(v_num, planet_rashis[0], v_lagn_rasi, planet_rashis, v_planets);
            analyze_varga_fate(v_num, v_lagn_rasi, v_planets);
            analyze_lordships(v_lagn_rasi, v_planets); 
            analyze_conjunctions(v_planets, v_lagn_rasi);
        }
    }

	void analyze_lordships(int lagna_rasi, int* p_rasi) {
        if (telugu_mode) printf("\n[భావ ఆధిపత్యాలు (పరాశర పద్ధతిలో ఫలితాలు)]\n");
        else printf("\n[BHAVA LORDSHIPS (Specific BPHS House Interpretations)]\n");
        
        for (int h = 1; h <= 12; h++) {
            int house_rashi = (lagna_rasi + h - 1) % 12;
            string lord_name = rashi_lords[house_rashi];
            
            int p_idx = -1;
            for (int i = 1; i <= 7; i++) { 
                if (string(p_names_full[i]) == lord_name) { p_idx = i; break; }
            }
            
            if (p_idx != -1) {
                int placed_h = (p_rasi[p_idx] - lagna_rasi + 12) % 12 + 1;
                
                if (telugu_mode) {
                    printf("  - %dవ భావ అధిపతి (%s) %dవ భావంలో ఉన్నాడు.\n", h, get_planet_name(p_idx).c_str(), placed_h);
                    printf("    * విశ్లేషణ: %s\n", te_get_lord_in_house_text(h, placed_h).c_str());
                } else {
                    printf("  - Lord of House %d (%s) is placed in House %d.\n", h, lord_name.c_str(), placed_h);
                    printf("    * Synthesis: %s\n", get_lord_in_house_text(h, placed_h).c_str());
                }
            }
        }
    }
	
	void analyze_auspiciousness(int lagna_rasi, int* p_rasi) {
        if (!json_mode) {
            if (telugu_mode) {
                printf("\n[సమగ్ర గ్రహ శుభ/అశుభ & పరిహార విశ్లేషణ (AUSPICIOUSNESS MATRIX)]\n");
                printf("-----------------------------------------------------------------------------------------------------------------\n");
                printf("%-10s | %-6s | %-15s | %-60s\n", "గ్రహం", "స్కోరు", "స్థితి", "వివరణాత్మక లెక్కింపు");
            } else {
                printf("\n[COMPREHENSIVE PLANETARY AUSPICIOUSNESS & REMEDY MATRIX]\n");
                printf("-----------------------------------------------------------------------------------------------------------------\n");
                printf("%-8s | %-6s | %-6s | %-60s\n", "Graha", "Score", "Status", "Detailed Calculation Breakdown");
            }
            printf("-----------------------------------------------------------------------------------------------------------------\n");
        }
        int d9_rashis[10];
        for(int i=1; i<=9; i++) d9_rashis[i] = get_varga(9, planet_lons[i]);

        double yogi_point = fmod((sun_lon + moon_lon + 93.3333333), 360.0);
        int y_nak_idx = (int)(yogi_point / (360.0 / 27.0));
        double avayogi_point = fmod((yogi_point + 186.6666667), 360.0);
        int ay_nak_idx = (int)(avayogi_point / (360.0 / 27.0));
        
        int lord_map[] = {9, 6, 1, 2, 3, 8, 5, 7, 4}; 
        int yogi_planet = lord_map[y_nak_idx % 9];
        int avayogi_planet = lord_map[ay_nak_idx % 9];
        int natal_mo_nak = (int)(moon_lon / (360.0 / 27.0));

        for (int p = 1; p <= 9; p++) {
            int score = 0; string breakdown = "";
            int exaltation_signs[] = {0, 0, 1, 9, 5, 3, 11, 6, 2, 7}; 
            int debilitation_signs[] = {0, 6, 7, 3, 11, 9, 5, 0, 8, 1}; 
            int own_signs1[] = {0, 4, 3, 0, 2, 8, 1, 9, -1, -1}; 
            int own_signs2[] = {0, -1, -1, 7, 5, 11, 6, 10, -1, -1};
            
            if (p <= 7) {
                if (p_rasi[p] == exaltation_signs[p]) { score += 4; breakdown += telugu_mode ? "ఉచ్ఛ(+4) " : "Exalted(+4) "; }
                else if (p_rasi[p] == debilitation_signs[p]) { score -= 3; breakdown += telugu_mode ? "నీచ(-3) " : "Debilitated(-3) "; }
                else if (p_rasi[p] == own_signs1[p] || p_rasi[p] == own_signs2[p]) { score += 3; breakdown += telugu_mode ? "స్వక్షేత్రం(+3) " : "Own Sign(+3) "; }
            }
            if (rvs_bad_placements[p][p_rasi[p]] == 1) { score -= 1; breakdown += telugu_mode ? "శత్రు/నీచ స్థానం(-1) " : "Enemy/Bad Rasi(-1) "; } 
            else if (p_rasi[p] != exaltation_signs[p] && p_rasi[p] != own_signs1[p] && p_rasi[p] != own_signs2[p]) { score += 1; breakdown += telugu_mode ? "మిత్ర స్థానం(+1) " : "Friendly Rasi(+1) "; }

            if (p <= 7) {
                bool rules_trikona = false, rules_dusthana = false, rules_kendra = false;
                for (int h = 1; h <= 12; h++) {
                    int rashi_of_house = (lagna_rasi + h - 1) % 12;
                    if (rashi_lords[rashi_of_house] == string(p_names_full[p])) {
                        if (h == 1 || h == 5 || h == 9) rules_trikona = true;
                        if (h == 3 || h == 6 || h == 8 || h == 11) rules_dusthana = true;
                        if (h == 4 || h == 7 || h == 10) rules_kendra = true;
                    }
                }
                if (rules_trikona) { score += 3; breakdown += telugu_mode ? "త్రికోణాధిపతి(+3) " : "Trikona Lord(+3) "; }
                if (rules_dusthana) { score -= 2; breakdown += telugu_mode ? "దుస్థానాధిపతి(-2) " : "Dusthana Lord(-2) "; }
                if (rules_kendra && !rules_trikona) { 
                    if (p == 2 || p == 4 || p == 5 || p == 6) { score -= 1; breakdown += telugu_mode ? "కేంద్రాధిపత్య దోషం(-1) " : "Kendradhipati Dosha(-1) "; }
                    if (p == 1 || p == 3 || p == 7) { score += 1; breakdown += telugu_mode ? "పాప కేంద్రాధిపతి(+1) " : "Malefic Kendra Lord(+1) "; }
                }
            }

            int h = (p_rasi[p] - lagna_rasi + 12) % 12 + 1;
            if (h == 1 || h == 5 || h == 9 || h == 4 || h == 7 || h == 10) { score += 2; breakdown += telugu_mode ? "శుభ భావ స్థితి(+2) " : "Good Placement(+2) "; }
            else if (h == 6 || h == 8 || h == 12) { score -= 3; breakdown += telugu_mode ? "దుస్థాన స్థితి(-3) " : "Dusthana Placement(-3) "; }
            else if (h == 3 || h == 10 || h == 11) { if (p == 1 || p == 3 || p == 7 || p == 8 || p == 9) { score += 1; breakdown += telugu_mode ? "ఉపచయంలో పాపి(+1) " : "Malefic in Upachaya(+1) "; } }
            
            int d9_h = (d9_rashis[p] - d9_rashis[0] + 12) % 12 + 1;
            if (d9_h == 6 || d9_h == 8 || d9_h == 12) { score -= 1; breakdown += telugu_mode ? "D9 దుస్థానం(-1) " : "D9 Dusthana(-1) "; }
            if (p_rasi[p] == d9_rashis[p]) { score += 2; breakdown += telugu_mode ? "వర్గోత్తమ(+2) " : "Vargottama(+2) "; }

            int nak_idx = (int)(planet_lons[p] / (360.0 / 27.0));
            int actual_nak_lord = lord_map[nak_idx % 9];
            if (natural_enemies[p][actual_nak_lord] == 1) { score -= 1; breakdown += telugu_mode ? "శత్రు నక్షత్రం(-1) " : "Enemy Nakshatra(-1) "; }

            int tara_idx = (nak_idx - natal_mo_nak + 27) % 9;
            if (tara_idx == 2 || tara_idx == 4 || tara_idx == 6) { score -= 1; breakdown += telugu_mode ? "ప్రతికూల తార(-1) " : "Bad Tara(-1) "; } 
            else if (tara_idx == 1 || tara_idx == 3 || tara_idx == 5 || tara_idx == 7 || tara_idx == 8) { score += 1; breakdown += telugu_mode ? "శుభ తార(+1) " : "Good Tara(+1) "; }

            if (p == yogi_planet) { score += 3; breakdown += telugu_mode ? "యోగి గ్రహం(+3) " : "YOGI Planet(+3) "; }
            if (p == avayogi_planet) { score -= 3; breakdown += telugu_mode ? "అవయోగి గ్రహం(-3) " : "AVAYOGI Planet(-3) "; }

            int malefic_influence = 0; int benefic_influence = 0; bool node_conjunction = false;
            for (int asp = 1; asp <= 9; asp++) {
                if (asp == p) continue;
                int dist = (p_rasi[p] - p_rasi[asp] + 12) % 12 + 1;
                bool is_interacting = false;
                
                if (dist == 1) { is_interacting = true; if (asp == 8 || asp == 9) node_conjunction = true; } 
                else if (dist == 7) is_interacting = true; 
                else if (asp == 3 && (dist == 4 || dist == 8)) is_interacting = true; 
                else if (asp == 5 && (dist == 5 || dist == 9)) is_interacting = true; 
                else if (asp == 7 && (dist == 3 || dist == 10)) is_interacting = true; 
                
                if (is_interacting) {
                    if (asp==1 || asp==3 || asp==7 || asp==8 || asp==9) malefic_influence++;
                    if (asp==2 || asp==4 || asp==5 || asp==6) benefic_influence++;
                }
            }
            if (node_conjunction) { score -= 2; breakdown += telugu_mode ? "ఛాయా గ్రహ కలయిక(-2) " : "Node Conjunction(-2) "; }
            if (malefic_influence > 0) { score -= malefic_influence; breakdown += (telugu_mode ? "పాప గ్రహ దృష్టి(-" : "Malefic Hit(-") + to_string(malefic_influence) + ") "; }
            if (benefic_influence > 0) { score += benefic_influence; breakdown += (telugu_mode ? "శుభ గ్రహ దృష్టి(+" : "Benefic Hit(+") + to_string(benefic_influence) + ") "; }

            string fusion_text = telugu_mode ? (score >= 3 ? "అత్యుత్తమ" : (score >= -1 && score <= 2 ? "సాధారణ" : "ప్రతికూల")) : (score >= 3 ? "BEST" : (score >= -1 && score <= 2 ? "AVERAGE" : "BAD"));
            double sb = 0.0;
            if (p >= 1 && p <= 7) { sb = ShadbalaEngine::final_ratios[p]; }
            else if (p == 8 || p == 9) {
                int disp_idx = 1;
                for(int d=1; d<=7; d++) { if (rashi_lords[p_rasi[p]] == string(p_names_full[d])) disp_idx = d; }
                sb = ShadbalaEngine::final_ratios[disp_idx];
                breakdown += telugu_mode ? ("(" + get_planet_name(disp_idx) + " బలాన్ని ఉపయోగిస్తుంది) ") : ("(Node utilizes " + string(p_names_full[disp_idx]) + "'s power) ");
            }

            if (sb > 0.0) {
                if (score >= 3) {
                    if (sb >= 1.1) fusion_text = telugu_mode ? "అఖండ విజయం (అధిక బలం + ఉత్తమ సంకల్పం)" : "MASSIVE SUCCESS (High Power + Best Intent)";
                    else if (sb >= 0.9) fusion_text = telugu_mode ? "గొప్ప ఫలితాలు (తగినంత బలం + ఉత్తమ సంకల్పం)" : "GREAT RESULTS (Adequate Power + Best Intent)";
                    else fusion_text = telugu_mode ? "ఉపయోగపడని శక్తి (ఉత్తమ సంకల్పం, కానీ బలం లేదు - రత్నధారణ చేయాలి)" : "UNREALIZED POTENTIAL (Best Intent, but lacks physical power - Use Gemstone/Color)";
                } else if (score <= -3) {
                    if (sb >= 1.1) fusion_text = telugu_mode ? "తీవ్ర ప్రమాదం/నష్టం! (అధిక బలం + చెడు సంకల్పం - తక్షణ పరిహారం అవసరం)" : "HIGH DESTRUCTION! (High Power + Bad Intent - URGENT REMEDY REQUIRED)";
                    else if (sb >= 0.9) fusion_text = telugu_mode ? "మధ్యస్థ ఘర్షణ (తగినంత బలం + చెడు సంకల్పం - పరిహారం అవసరం)" : "MODERATE FRICTION (Adequate Power + Bad Intent - Remedy Needed)";
                    else fusion_text = telugu_mode ? "చిన్నపాటి చికాకు (చెడు సంకల్పం ఉన్నా, హాని చేసే బలం లేదు)" : "MINOR ANNOYANCE (Bad Intent, but lacks the physical power to cause major harm)";
                } else {
                    if (sb >= 1.1) fusion_text = telugu_mode ? "శక్తివంతమైనది కానీ అస్థిరమైనది (అధిక బలం + మిశ్రమ సంకల్పం)" : "POWERFUL BUT VOLATILE (High Power + Mixed Intent)";
                    else if (sb >= 0.9) fusion_text = telugu_mode ? "సాధారణం (సాధారణ బలం + మిశ్రమ సంకల్పం)" : "AVERAGE (Standard Power + Mixed Intent)";
                    else fusion_text = telugu_mode ? "బలహీనమైనది & ఊహించనిది (తక్కువ బలం + మిశ్రమ సంకల్పం)" : "WEAK & UNPREDICTABLE (Low Power + Mixed Intent)";
                }
            }
            natal_scores[p] = score; 
            if (!json_mode) {
                if (telugu_mode) printf("%-10s | %-6d | %-40s | %s\n", get_planet_name(p).c_str(), score, fusion_text.c_str(), breakdown.c_str());
                else printf("%-8s | %-6d | %-65s | %s\n", p_names_full[p], score, fusion_text.c_str(), breakdown.c_str());
            }
        }
        if (!json_mode) {
            printf("-----------------------------------------------------------------------------------------------------------------\n");
            if (telugu_mode) {
                printf(" * గమనిక: 'ప్రతికూల/ప్రమాదకర' అని ఉన్న గ్రహాలకు జపాలు/దానాలు వంటి నిర్దిష్ట పరిహారాలు అవసరం.\n");
                printf(" * గమనిక: 'ఉపయోగపడని శక్తి' అని ఉన్న గ్రహాలకు రత్నధారణ/యంత్రాల ద్వారా బలాన్ని పెంచాలి.\n");
            } else {
                printf(" * NOTE: Planets marked 'BAD/DESTRUCTION' require specific Remedies (Mantras/Daanams).\n");
                printf(" * NOTE: Planets marked 'UNREALIZED POTENTIAL' require Strengthening (Gemstones/Metals).\n");
            }
        }
    }

void search_exact_degree(string planet_name, string sign_name, int deg, int min, int sec, int search_year, int search_month) {
        // 1. Resolve Planet Index
        string p_lower = planet_name;
        transform(p_lower.begin(), p_lower.end(), p_lower.begin(), ::tolower);
        int p_idx = -1;
        if (p_lower == "surya" || p_lower == "sun" || p_lower == "ravi") p_idx = 1;
        else if (p_lower == "chandra" || p_lower == "moon") p_idx = 2;
        else if (p_lower == "mangal" || p_lower == "mars" || p_lower == "kuja") p_idx = 3;
        else if (p_lower == "budha" || p_lower == "mercury") p_idx = 4;
        else if (p_lower == "guru" || p_lower == "jupiter" || p_lower == "brihaspati") p_idx = 5;
        else if (p_lower == "shukra" || p_lower == "venus" || p_lower == "sukra") p_idx = 6;
        else if (p_lower == "shani" || p_lower == "saturn") p_idx = 7;
        else if (p_lower == "rahu") p_idx = 8;
        else if (p_lower == "ketu") p_idx = 9;
        
        if (p_idx == -1) { printf("Error: Planet '%s' not recognized.\n", planet_name.c_str()); return; }

        // 2. Resolve Sign Index
        string s_lower = sign_name;
        transform(s_lower.begin(), s_lower.end(), s_lower.begin(), ::tolower);
        int s_idx = -1;
        if (s_lower == "mesha" || s_lower == "mesh") s_idx = 0;
        else if (s_lower == "vrishabha" || s_lower == "vrish") s_idx = 1;
        else if (s_lower == "mithuna" || s_lower == "mitu") s_idx = 2;
        else if (s_lower == "karka" || s_lower == "karkataka") s_idx = 3;
        else if (s_lower == "simha" || s_lower == "simh") s_idx = 4;
        else if (s_lower == "kanya" || s_lower == "kany") s_idx = 5;
        else if (s_lower == "tula") s_idx = 6;
        else if (s_lower == "vrishchika" || s_lower == "vrischika") s_idx = 7;
        else if (s_lower == "dhanu" || s_lower == "dhanus") s_idx = 8;
        else if (s_lower == "makara" || s_lower == "makar") s_idx = 9;
        else if (s_lower == "kumbha" || s_lower == "kumbh") s_idx = 10;
        else if (s_lower == "meena" || s_lower == "meen") s_idx = 11;

        if (s_idx == -1) { printf("Error: Sign '%s' not recognized.\n", sign_name.c_str()); return; }

        // 3. Calculate Target Absolute Longitude
        double target_lon = (s_idx * 30.0) + deg + (min / 60.0) + (sec / 3600.0);

        // 4. Set Timeframe boundaries
        double start_jd, end_jd;
        string scope_str = "";
        
        if (search_year > 0 && search_month > 0) {
            start_jd = swe_julday(search_year, search_month, 1, 0.0 - location.tz_offset, SE_GREG_CAL);
            int next_m = search_month == 12 ? 1 : search_month + 1;
            int next_y = search_month == 12 ? search_year + 1 : search_year;
            end_jd = swe_julday(next_y, next_m, 1, 0.0 - location.tz_offset, SE_GREG_CAL);
            scope_str = "Month: " + to_string(search_month) + "/" + to_string(search_year);
        } else if (search_year > 0) {
            start_jd = swe_julday(search_year, 1, 1, 0.0 - location.tz_offset, SE_GREG_CAL);
            end_jd = swe_julday(search_year + 1, 1, 1, 0.0 - location.tz_offset, SE_GREG_CAL);
            scope_str = "Year: " + to_string(search_year);
        } else {
            start_jd = tjd_ut; 
            end_jd = start_jd + (120.0 * 365.2425);
            scope_str = "LIFESPAN (120 Years)";
        }

        printf("\n=== EXACT DEGREE SEARCH ===\n");
        printf("Target: %s arriving at %02d° %s %02d'%02d\"\n", p_names_full[p_idx], deg, rashi_names[s_idx], min, sec);
        printf("Scope: %s\n", scope_str.c_str());
        printf("---------------------------------------------------------------------------------\n");
        printf("%-20s | %-15s | %-20s\n", "Exact Date & Time", "Planet Status", "Movement");
        printf("---------------------------------------------------------------------------------\n");

        double step = (p_idx == 2) ? 0.25 : 1.0; 
        int hits = 0;
        
        for (double jd = start_jd; jd < end_jd; jd += step) {
            // FIX: Unified position getter for exact degrees
            double lon1 = get_planet_lon_on_jd(p_idx, jd);
            double lon2 = get_planet_lon_on_jd(p_idx, jd + step);
            
            double d1 = fmod(lon1 - target_lon + 360.0, 360.0); if (d1 > 180.0) d1 -= 360.0;
            double d2 = fmod(lon2 - target_lon + 360.0, 360.0); if (d2 > 180.0) d2 -= 360.0;
            
            if (d1 * d2 <= 0.0 && std::abs(d1) < 15.0) { 
                
                double t_low = jd, t_high = jd + step;
                for (int iter = 0; iter < 40; iter++) { 
                    double t_mid = (t_low + t_high) / 2.0;
                    double lon_mid = get_planet_lon_on_jd(p_idx, t_mid);
                    
                    double d_mid = fmod(lon_mid - target_lon + 360.0, 360.0); if (d_mid > 180.0) d_mid -= 360.0;
                    
                    if (d1 * d_mid <= 0.0) {
                        t_high = t_mid; 
                    } else {
                        t_low = t_mid; 
                        d1 = d_mid;
                    }
                }
                
                double exact_jd = (t_low + t_high) / 2.0;
                
                // Extract Retrograde or Direct movement
                double d_move = fmod(lon2 - lon1 + 360.0, 360.0);
                if (d_move > 180.0) d_move -= 360.0;
                string dir = (d_move < 0) ? "Retrograde (Vakra)" : "Direct (Margi)";
                
                string impact = "Neutral";
                for(int n=1; n<=9; n++) {
                    if (std::abs(fmod(planet_lons[n], 360.0) - target_lon) < 2.0) {
                        impact = (natal_scores[n] < 0) ? "URGENT HIT (" + string(p_names_full[n]) + ")" : "OPPORTUNITY (" + string(p_names_full[n]) + ")";
                    }
                }

                printf("%-20s | %-15s | %-20s\n", jd_to_string(exact_jd).c_str(), impact.c_str(), dir.c_str());
                hits++;
            }
        }
        
        if (hits == 0) printf("No events found in the specified timeframe.\n");
        printf("---------------------------------------------------------------------------------\n");
    }
	
	void scan_planetary_collisions(string target_planet, int start_year, int start_month, int start_day) {
        vector<int> t_targets;
        string p_lower = target_planet;
        transform(p_lower.begin(), p_lower.end(), p_lower.begin(), ::tolower);
        
        if (p_lower == "all" || p_lower == "") {
            for (int i = 1; i <= 9; i++) t_targets.push_back(i);
        } else {
            int p_idx = -1;
            if (p_lower == "surya" || p_lower == "sun" || p_lower == "ravi") p_idx = 1;
            else if (p_lower == "chandra" || p_lower == "moon") p_idx = 2;
            else if (p_lower == "mangal" || p_lower == "mars" || p_lower == "kuja") p_idx = 3;
            else if (p_lower == "budha" || p_lower == "mercury") p_idx = 4;
            else if (p_lower == "guru" || p_lower == "jupiter" || p_lower == "brihaspati") p_idx = 5;
            else if (p_lower == "shukra" || p_lower == "venus" || p_lower == "sukra") p_idx = 6;
            else if (p_lower == "shani" || p_lower == "saturn") p_idx = 7;
            else if (p_lower == "rahu") p_idx = 8;
            else if (p_lower == "ketu") p_idx = 9;
            
            if (p_idx == -1) { printf("Error: Planet '%s' not recognized.\n", target_planet.c_str()); return; }
            t_targets.push_back(p_idx);
        }

        // --- DYNAMIC TIME WINDOW LOGIC ---
        double start_jd, end_jd;
        string scope_desc;
        
        if (start_year > 0 && start_month > 0 && start_day > 0) {
            start_jd = swe_julday(start_year, start_month, start_day, 0.0 - location.tz_offset, SE_GREG_CAL);
            end_jd = start_jd + 1.0; 
            char buf[64]; sprintf(buf, "%02d/%02d/%04d", start_day, start_month, start_year);
            scope_desc = "Exact Day: " + string(buf);
        } 
        else if (start_year > 0 && start_month > 0) {
            start_jd = swe_julday(start_year, start_month, 1, 0.0 - location.tz_offset, SE_GREG_CAL);
            int next_m = (start_month == 12) ? 1 : start_month + 1;
            int next_y = (start_month == 12) ? start_year + 1 : start_year;
            end_jd = swe_julday(next_y, next_m, 1, 0.0 - location.tz_offset, SE_GREG_CAL);
            char buf[64]; sprintf(buf, "%02d/%04d", start_month, start_year);
            scope_desc = "Month: " + string(buf);
        } 
        else {
            int y = (start_year > 0) ? start_year : 2026;
            start_jd = swe_julday(y, 1, 1, 0.0 - location.tz_offset, SE_GREG_CAL);
            end_jd = swe_julday(y + 1, 1, 1, 0.0 - location.tz_offset, SE_GREG_CAL);
            scope_desc = "Year: " + to_string(y);
        }

        printf("\n=== GLOBAL PRECISION TRANSIT SCANNER (YUTI & VEDIC DRISHTI) ===\n");
        printf("Scope: %s | Target: %s\n", scope_desc.c_str(), (p_lower == "all" || p_lower == "") ? "All Planets" : p_names_full[t_targets[0]]);
        printf("%-10s | %-10s | %-14s | %-20s | %-20s | %-20s | %-35s\n", 
               "Transit", "Natal", "Aspect Type", "Enter Time", "Peak Time", "Exit Time", "Status & Reason");
        printf("----------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

        const double orb = 2.0;
        struct AspectTarget { double lon; string name; };

        for (int t : t_targets) {
            // V9.2 FIX: Start loop at 0 to include the Natal Lagna (Ascendant)
            for (int n = 0; n <= 9; n++) {
                
                vector<AspectTarget> targets;
                double n_lon = planet_lons[n];
                
                // Vedic Drishti Mapping
                targets.push_back({n_lon, "1st (Yuti)"});
                targets.push_back({fmod(n_lon + 180.0, 360.0), "7th Aspect"});
                
                if (t == 3) { 
                    targets.push_back({fmod(n_lon + 270.0, 360.0), "4th Aspect"}); 
                    targets.push_back({fmod(n_lon + 150.0, 360.0), "8th Aspect"}); 
                }
                else if (t == 5) { 
                    targets.push_back({fmod(n_lon + 240.0, 360.0), "5th Aspect"}); 
                    targets.push_back({fmod(n_lon + 120.0, 360.0), "9th Aspect"}); 
                }
                else if (t == 7) { 
                    targets.push_back({fmod(n_lon + 300.0, 360.0), "3rd Aspect"}); 
                    targets.push_back({fmod(n_lon + 90.0, 360.0),  "10th Aspect"}); 
                }
                else if (t == 8 || t == 9) { 
                    targets.push_back({fmod(n_lon + 240.0, 360.0), "5th Aspect"});
                    targets.push_back({fmod(n_lon + 120.0, 360.0), "9th Aspect"});
                }

                for (const auto& tgt : targets) {
                    int target_rashi = (int)(tgt.lon / 30.0);
                    double step = (t == 2) ? 1.0 / 24.0 : 4.0 / 24.0; 
                    
                    for (double jd = start_jd; jd < end_jd; jd += step) {
                        
                        double trans_lon = get_planet_lon_on_jd(t, jd);
                        
                        if ((int)(trans_lon / 30.0) != target_rashi) continue; 
                        
                        double dist = std::abs(fmod(trans_lon, 30.0) - fmod(tgt.lon, 30.0));
                        
                        if (dist <= orb) {
                            double e_in, e_peak, e_out;
                            refine_bubble(t, tgt.lon, jd, orb, e_in, e_peak, e_out);

                            // --- DYNAMIC REASON GENERATION ---
                            string impact = "";
                            if (n == 0) {
                                // Lagna Hit Context: Depends entirely on the Transiting Planet's nature
                                bool t_is_malefic = (t == 1 || t == 3 || t == 7 || t == 8 || t == 9);
                                string reason = "[Lagna / Core Physical Body Hit]";
                                impact = (t_is_malefic ? "DANGER " : "OPPORTUNITY ") + reason;
                            } else {
                                // Standard Planetary Hit Context: Depends on the Natal Planet's dignity score
                                string severity = (natal_scores[n] <= -5) ? "High Malefic" : 
                                                  (natal_scores[n] < 0)   ? "Malefic" : 
                                                  (natal_scores[n] >= 5)  ? "High Benefic" : "Neutral/Benefic";
                                
                                string reason = "[Score: " + to_string(natal_scores[n]) + ", " + severity + "]";
                                impact = (natal_scores[n] < 0) ? "DANGER " + reason : "OPPORTUNITY " + reason;
                            }
                            
                            string natal_name = (n == 0) ? "Lagna" : string(p_names_full[n]);
                            
                            printf("%-10s | %-10s | %-14s | %-20s | %-20s | %-20s | %-35s\n", 
                                   p_names_full[t], natal_name.c_str(), tgt.name.c_str(),
                                   jd_to_string(e_in).c_str(), jd_to_string(e_peak).c_str(), 
                                   jd_to_string(e_out).c_str(), impact.c_str());
                            
                            if (e_out > jd) jd = e_out; // Safeguard against infinity loop desyncs
                        }
                    }
                }
            }
        }
        printf("----------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
    }	
    void analyze_varga_synthesis(int v_num, int d1_lagna, int v_lagna, int* d1_rasi, int* v_rasi) {
        printf("\n[D1 TO VARGA SUPERIMPOSITION (Cross-Reference Matrix)]\n");
        
        // 1. Vargottama Check
        bool varg_found = false;
        for (int i = 1; i <= 9; i++) {
            if (d1_rasi[i] == v_rasi[i]) {
                if (!varg_found) printf("  - DYNAMIC CRYSTALLIZATION (Vargottama Planets):\n");
                printf("    * %s\n", get_vargottama_text(p_names_full[i]).c_str());
                varg_found = true;
            }
        }
        if (!varg_found) printf("  - No Vargottama planets detected in this specific alignment.\n");

        // 2. D1 Lagna Lord Insertion
        int d1_lord_idx = -1;
        for (int p=1; p<=7; p++) {
            if (rashi_lords[d1_lagna] == string(p_names_full[p])) d1_lord_idx = p;
        }
        if (d1_lord_idx != -1) {
            int h = (v_rasi[d1_lord_idx] - v_lagna + 12) % 12 + 1;
            printf("\n  - D1 AVATAR INSERTION (Where your core physical self applies its focus here):\n");
            printf("    * %s (Your D1 Lagna Lord) is placed in House %d of this Varga.\n", p_names_full[d1_lord_idx], h);
            printf("    * Synthesis: %s\n", get_d1_lord_in_varga_text(p_names_full[d1_lord_idx], h).c_str());
        }

        // 3. The Natural Karaka (Significator) of the Varga
        int karaka_idx = get_varga_karaka(v_num);
        if (karaka_idx != -1) {
            int h = (v_rasi[karaka_idx] - v_lagna + 12) % 12 + 1;
            printf("\n  - VARGA KARAKA (The Natural Governor of this Chart):\n");
            printf("    * %s governs the D%d. It is placed in House %d of this Varga.\n", p_names_full[karaka_idx], v_num, h);
            if (h == 6 || h == 8 || h == 12) printf("    * Warning: The natural governor is placed in a Dusthana, indicating inherent structural weakness or hidden struggles regarding these specific themes.\n");
            else if (h == 1 || h == 4 || h == 5 || h == 7 || h == 9 || h == 10) printf("    * Strength: The natural governor is placed in a foundational house, ensuring the core themes of this chart are well-supported.\n");
        }
    }

	void analyze_functional_nature(int lagna_rasi) {
        if (telugu_mode) printf("\n[నైసర్గిక స్వభావం (%s లగ్నం ఆధారంగా)]\n", get_rashi_name(lagna_rasi).c_str());
        else printf("\n[FUNCTIONAL NATURE (Based on %s Lagna Lordship)]\n", rashi_names[lagna_rasi]);
        
        for (int p=1; p<=7; p++) {
            bool is_benefic = false, is_malefic = false, is_kendra = false;
            for (int h=1; h<=12; h++) {
                int rashi_of_house = (lagna_rasi + h - 1) % 12;
                if (rashi_lords[rashi_of_house] == string(p_names_full[p])) {
                    if (h==1 || h==5 || h==9) is_benefic = true;
                    if (h==3 || h==6 || h==11) is_malefic = true;
                    if (h==4 || h==7 || h==10) is_kendra = true;
                }
            }
            string status = telugu_mode ? "తటస్థ / మిశ్రమ" : "Neutral / Mixed";
            if (is_benefic && !is_malefic) status = telugu_mode ? "నైసర్గిక శుభ గ్రహం (అత్యంత అనుకూలం)" : "Functional Benefic (Auspicious)";
            if (!is_benefic && is_malefic) status = telugu_mode ? "నైసర్గిక పాప గ్రహం (ప్రతికూలం)" : "Functional Malefic (Challenging)";
            if (is_benefic && is_kendra) status = telugu_mode ? "రాజయోగ కారకుడు (అత్యంత శుభకరం)" : "Yogakaraka (Highly Auspicious)";
            
            if (telugu_mode) printf("  - %-10s: %s\n", get_planet_name(p).c_str(), status.c_str());
            else printf("  - %-8s: %s\n", p_names_full[p], status.c_str());
        }
        
        int r_rahu = planet_rashis[8]; int r_ketu = planet_rashis[9];
        auto get_lord_te = [&](string en_name) { for(int i=1; i<=7; i++) { if(en_name == p_names_full[i]) return string(te_p_names_full[i]); } return en_name; };

        if (telugu_mode) {
            printf("  - రాహువు    : ఛాయా గ్రహాలు తమ అధిపతుల ద్వారా పనిచేస్తాయి. రాహువు %s నియంత్రణలో ఉన్నాడు.\n", get_lord_te(rashi_lords[r_rahu]).c_str());
            printf("  - కేతువు     : ఛాయా గ్రహాలు తమ అధిపతుల ద్వారా పనిచేస్తాయి. కేతువు %s నియంత్రణలో ఉన్నాడు.\n", get_lord_te(rashi_lords[r_ketu]).c_str());
        } else {
            printf("  - %-8s: Shadow Nodes operate via their dispositors. Rahu is governed by %s.\n", "Rahu", rashi_lords[r_rahu]);
            printf("  - %-8s: Shadow Nodes operate via their dispositors. Ketu is governed by %s.\n", "Ketu", rashi_lords[r_ketu]);
        }
    }
	
	void analyze_final_outcomes(int lagna_rasi, int* p_rasi) {
        if (telugu_mode) printf("\n[గ్రహాల తుది ఫలితం (దశ/అంతర్దశలలో జరిగేవి)]\n");
        else printf("\n[SYNTHESIZED FINAL OUTCOME OF PLANETS (D1 FATE)]\n");
        
        for (int p=1; p<=9; p++) {
            int h = (p_rasi[p] - lagna_rasi + 12) % 12 + 1;
            bool is_dusthana = (h==6 || h==8 || h==12);
            bool is_kendra_trikona = (h==1 || h==4 || h==5 || h==7 || h==9 || h==10);
            
            bool is_benefic = false, is_malefic = false, is_kendra_lord = false;
            if (p <= 7) {
                for (int hs=1; hs<=12; hs++) {
                    int rashi_of_house = (lagna_rasi + hs - 1) % 12;
                    if (rashi_lords[rashi_of_house] == string(p_names_full[p])) {
                        if (hs==1 || hs==5 || hs==9) is_benefic = true;
                        if (hs==3 || hs==6 || hs==11) is_malefic = true;
                        if (hs==4 || hs==7 || hs==10) is_kendra_lord = true;
                    }
                }
            }

            int disp_idx = 1;
            for(int d=1; d<=7; d++) { if (rashi_lords[p_rasi[p]] == string(p_names_full[d])) disp_idx = d; }
            
            if (telugu_mode) {
                string te_disp_name = get_planet_name(disp_idx);
                string outcome = te_get_final_outcome(p, is_benefic, is_malefic, is_kendra_lord, is_dusthana, is_kendra_trikona, h, te_disp_name);
                printf("  - %-12s: %s\n", get_planet_name(p).c_str(), outcome.c_str());
            } else {
                string disp_name = p_names_full[disp_idx];
                string outcome = get_final_outcome(p, is_benefic, is_malefic, is_kendra_lord, is_dusthana, is_kendra_trikona, h, disp_name);
                printf("  - %-8s: %s\n", p_names_full[p], outcome.c_str());
            }
        }
    }
    void analyze_varga_fate(int v_num, int v_lagna, int* p_rasi) {
        printf("\n[MICRO-CHART FATE & ENGAGEMENT]\n");
        printf("  - %s\n", get_varga_theme(v_num).c_str());
        printf("  - Micro-Environment (Varga Lagna): %s\n\n", rashi_names[v_lagna]);

        for (int i=1; i<=9; i++) {
            int h = (p_rasi[i] - v_lagna + 12) % 12 + 1;
            printf("  - %s is located in House %d of this Varga.\n", p_names_full[i], h);
            printf("    * %s\n", get_varga_engagement(h).c_str());
        }
    }

    void analyze_yogas(int* p_rasi, int lagna) {
        if (telugu_mode) printf("\n[ప్రధాన యోగాలు (రాజయోగాలు)]\n");
        else printf("\n[MAJOR YOGAS DETECTED]\n");
        
        bool yoga_found = false;
        auto in_kendra = [&](int r, int l) { int h = (r - l + 12) % 12 + 1; return (h==1 || h==4 || h==7 || h==10); };
        auto is_own_exalt = [&](int r, int ex, int own1, int own2) { return (r==ex || r==own1 || r==own2); };

        auto print_yoga = [&](string y_name, string en_text) {
            if (telugu_mode) printf("  - %s\n", te_get_yoga_text(y_name).c_str());
            else printf("  - %s\n", en_text.c_str());
            yoga_found = true;
        };

        if (in_kendra(p_rasi[3], lagna) && is_own_exalt(p_rasi[3], 9, 0, 7)) print_yoga("Ruchaka", "Ruchaka Yoga: Mars is powerfully placed in a Kendra in its own or exalted sign. This grants profound courage, natural leadership, and heavy success in real estate or technical domains.");
        if (in_kendra(p_rasi[4], lagna) && is_own_exalt(p_rasi[4], 5, 2, 5)) print_yoga("Bhadra", "Bhadra Yoga: Mercury is powerfully placed in a Kendra... grants sharp intellect, flawless communication, and business acumen.");
        if (in_kendra(p_rasi[5], lagna) && is_own_exalt(p_rasi[5], 3, 8, 11)) print_yoga("Hamsa", "Hamsa Yoga: Jupiter is powerfully placed in a Kendra... surrounds the native with an aura of deep wisdom and spiritual respect.");
        if (in_kendra(p_rasi[6], lagna) && is_own_exalt(p_rasi[6], 11, 1, 6)) print_yoga("Malavya", "Malavya Yoga: Venus is powerfully placed in a Kendra... guarantees a life immersed in luxury, fine arts, and magnetism.");
        if (in_kendra(p_rasi[7], lagna) && is_own_exalt(p_rasi[7], 6, 9, 10)) print_yoga("Sasa", "Sasa Yoga: Saturn is powerfully placed in a Kendra... grants unbreakable persistence and the ability to hold vast authority.");
        if (in_kendra(p_rasi[5], p_rasi[2])) print_yoga("Gajakesari", "Gajakesari Yoga: Jupiter forms a powerful angular relationship with the Moon... imparts lasting reputation and profound resilience.");
        if (p_rasi[1] == p_rasi[4]) print_yoga("Budhaditya", "Budhaditya Yoga: The Sun and Mercury are conjunct... creates a highly analytical, brilliantly sharp mind.");
        if (p_rasi[2] == p_rasi[3]) print_yoga("ChandraMangala", "Chandra-Mangala Yoga: The Moon and Mars are conjunct... generates restless emotional intensity geared towards financial drive.");
        
        if(!yoga_found) {
            if (telugu_mode) printf("  - ఈ జాతక చక్రంలో ప్రధాన మహాపురుష యోగాలు ఏవీ గుర్తించబడలేదు.\n");
            else printf("  - No major primary Mahapurusha yogas detected in this specific alignment.\n");
        }
    }

    void analyze_doshas(int* p_rasi, int lagna) {
        if (telugu_mode) printf("\n[ప్రధాన దోషాలు (గమనించాల్సినవి)]\n");
        else printf("\n[MAJOR DOSHAS DETECTED]\n");
        
        bool dosha_found = false;
        int ma_h_lagna = (p_rasi[3] - lagna + 12) % 12 + 1;
        int ma_h_moon = (p_rasi[3] - p_rasi[2] + 12) % 12 + 1;
        bool lagna_kuja = (ma_h_lagna==1 || ma_h_lagna==2 || ma_h_lagna==4 || ma_h_lagna==7 || ma_h_lagna==8 || ma_h_lagna==12);
        bool moon_kuja = (ma_h_moon==1 || ma_h_moon==2 || ma_h_moon==4 || ma_h_moon==7 || ma_h_moon==8 || ma_h_moon==12);

        if (lagna_kuja || moon_kuja) {
            if (telugu_mode) printf("  - %s\n", te_get_dosha_text("Kuja", ma_h_lagna, ma_h_moon).c_str());
            else {
                printf("  - Mangal (Kuja) Dosha: Mars occupies House %d from the Lagna and House %d from the Moon.\n", ma_h_lagna, ma_h_moon);
                printf("    * Effect: This aggressive energy creates intense friction within partnerships. Mitigated if partner has it, or after age 28.\n");
            }
            dosha_found = true;
        }

        int r_rahu = p_rasi[8], r_ketu = p_rasi[9];
        bool all_one_side = true, all_other_side = true;
        for (int i=1; i<=7; i++) {
            int d1 = (p_rasi[i] - r_rahu + 12) % 12;
            int d2 = (r_ketu - r_rahu + 12) % 12;
            if (d1 > d2) all_one_side = false;
            if (d1 < d2 && d1 != 0) all_other_side = false; 
        }
        if (all_one_side || all_other_side) {
            if (telugu_mode) printf("  - %s\n", te_get_dosha_text("KalaSarpa", 0, 0).c_str());
            else {
                printf("  - Kala Sarpa Matrix: All major physical planets are physically hemmed within the Rahu/Ketu karmic axis.\n");
                printf("    * Effect: Enforces delays in the first half of life, building immense pressure that releases into success later.\n");
            }
            dosha_found = true;
        }
        
        if(!dosha_found) {
            if (telugu_mode) printf("  - ఈ జాతకంలో ఎటువంటి ప్రధాన నిర్మాణ దోషాలు లేవు.\n");
            else printf("  - No major structural doshas detected.\n");
        }
    }

	void analyze_placements(int* p_rasi, int lagna) {
        if (telugu_mode) printf("\n[గ్రహ స్థానాలు & ఫలితాలు]\n");
        else printf("\n[PLANETARY PLACEMENTS & EFFECTS]\n");
        
        for (int i=1; i<=9; i++) {
            int h = (p_rasi[i] - lagna + 12) % 12 + 1;
            
            if (telugu_mode) {
                printf("  - %s %dవ భావంలో (%s) ఉంది:\n", get_planet_name(i).c_str(), h, get_short_rashi(p_rasi[i]).c_str());
                printf("    * విశ్లేషణ: %s\n", te_get_planet_in_house_text(i, h).c_str());
                string digbala = te_get_digbala_text(i, h);
                if (digbala != "") printf("   %s\n", digbala.c_str());
            } else {
                printf("  - %s is located in House %d (%s):\n", p_names_full[i], h, short_rashi[p_rasi[i]]);
                printf("    * Synthesis: %s\n", get_planet_in_house_text(i, h).c_str());
                string digbala = get_digbala_text(i, h);
                if (digbala != "") printf("   %s\n", digbala.c_str());
            }
        }
    }
    
	void analyze_conjunctions(int* p_rasi, int lagna) {
        if (telugu_mode) printf("\n[గ్రహ కలయికలు (యుతి)]\n");
        else printf("\n[PLANETARY CONJUNCTIONS]\n");
        
        map<int, vector<int>> houses;
        for (int i=1; i<=9; i++) {
            int h = (p_rasi[i] - lagna + 12) % 12 + 1;
            houses[h].push_back(i);
        }

        bool found = false;
        for (auto const& [h, planets] : houses) {
            if (planets.size() > 1) {
                found = true;
                if (telugu_mode) printf("  - %dవ భావంలో ఈ కింది గ్రహాలు కలిసి ఉన్నాయి: ", h);
                else printf("  - House %d is heavily populated by: ", h);
                
                for (size_t j=0; j<planets.size(); j++) {
                    if (telugu_mode) printf("%s", get_planet_name(planets[j]).c_str());
                    else printf("%s", p_names_full[planets[j]]);
                    if (j < planets.size()-1) printf(" + ");
                }
                printf("\n");
                
                bool has_rahu_ketu = false;
                for(size_t j=0; j<planets.size(); j++) {
                    if(planets[j]==8 || planets[j]==9) has_rahu_ketu = true;
                }
                
                if (telugu_mode) {
                    if(has_rahu_ketu) printf("    * ప్రభావం: ఇక్కడ ఛాయా గ్రహం (రాహు/కేతు) ఉండటం వల్ల మిగతా గ్రహాల సహజ శక్తి దెబ్బతింటుంది లేదా విపరీతంగా అంచనాలకు మించి పనిచేస్తుంది.\n");
                    else printf("    * ప్రభావం: ఈ గ్రహ శక్తులు శాశ్వతంగా కలిసిపోవడం వల్ల, జీవితంలోని ఈ రంగంలో జాతకుడు ఎప్పుడూ భిన్నమైన పరిస్థితులను బ్యాలెన్స్ చేసుకోవాల్సి వస్తుంది.\n");
                } else {
                    if(has_rahu_ketu) printf("    * Effect: The presence of a Shadow Node acts as a distorting amplifier. It will heavily eclipse, exaggerate, or destabilize the physical planets trapped here with it.\n");
                    else printf("    * Effect: These planetary energies are permanently fused together, forcing the native to constantly balance their competing significations within this area of life.\n");
                }
            }
        }
        if(!found) {
            if (telugu_mode) printf("  - గ్రహ కలయికలు ఏవీ లేవు. అన్ని గ్రహాలు స్వత衡ంగా పనిచేస్తున్నాయి.\n");
            else printf("  - No planetary conjunctions found. All planets operate independently.\n");
        }
    }
	
    // =========================================================================
    // ORIGINAL CORE FUNCTIONS RESTOR	ED (Navatara, Karakas, etc.)
    // =========================================================================
    void calculate_navatara_table() {
        if (json_mode) return;
        int natal_mo_nak = (int)(moon_lon / (360.0 / 27.0));

        printf("\n=== NAVATARA (TARA BALA) GROUPS ===\n");
        printf("----------------------------------------------------------------------------------------------------\n");
        printf("%-15s | %-20s | %-60s\n", "Group (Tara)", "Nature", "Nakshatras");
        printf("----------------------------------------------------------------------------------------------------\n");
        for (int t = 0; t < 9; t++) {
            string naks = ""; int count = 0;
            for (int i = 0; i < 27; i++) {
                if ((i - natal_mo_nak + 27) % 9 == t) {
                    if (count > 0) naks += ", "; naks += nak_names[i]; count++;
                }
            }
            string t_name = tara_names[t];
            string group = t_name.substr(0, t_name.find(" ("));
            string nature = t_name.substr(t_name.find("(") + 1);
            nature.pop_back(); 
            printf("%-15s | %-20s | %-60s\n", group.c_str(), nature.c_str(), naks.c_str());
        }
        printf("----------------------------------------------------------------------------------------------------\n");
    }

	void calculate_special_karakas() {
        if (json_mode) return;
        vector<Karaka> k_list;
        for (int i = 1; i <= 7; i++) k_list.push_back({i-1, fmod(planet_lons[i], 30.0)});
        sort(k_list.begin(), k_list.end(), [](const Karaka& a, const Karaka& b) { return a.deg > b.deg; });

        double yogi_point = fmod((sun_lon + moon_lon + 93.3333333), 360.0);
        int y_nak_idx = (int)(yogi_point / (360.0 / 27.0));
        
        double avayogi_point = fmod((yogi_point + 186.6666667), 360.0);
        int ay_nak_idx = (int)(avayogi_point / (360.0 / 27.0));
        
        double avayogi_ni_point = fmod((yogi_point + 80.0), 360.0);
        int ay_ni_nak_idx = (int)(avayogi_ni_point / (360.0 / 27.0));

        printf("\n=== SPECIAL KARAKAS & YOGI POINTS ===\n");
        printf("-----------------------------------------------------------------\n");
        printf("Atmakaraka (AK)    : %s (%.2f°)\n", p_names_full[k_list[0].p_idx + 1], k_list[0].deg);
        printf("Amatyakaraka (AmK) : %s (%.2f°)\n", p_names_full[k_list[1].p_idx + 1], k_list[1].deg);
        printf("-----------------------------------------------------------------\n");
        printf("Yogi Point         : %s\n", format_dms(yogi_point).c_str());
        printf("Yogi Nakshatra     : %s\n", nak_names[y_nak_idx]);
        printf("Yogi Planet        : %s\n", dasha_lords[y_nak_idx % 9]);
        printf("-----------------------------------------------------------------\n");
        printf("Avayogi Point      : %s\n", format_dms(avayogi_point).c_str());
        printf("Avayogi Nakshatra  : %s\n", nak_names[ay_nak_idx]);
        printf("Avayogi Planet     : %s\n", dasha_lords[ay_nak_idx % 9]);
        printf("-----------------------------------------------------------------\n");
        printf("NI Avayogi Point   : %s\n", format_dms(avayogi_ni_point).c_str());
        printf("NI Avayogi Nak     : %s\n", nak_names[ay_ni_nak_idx]);
        printf("NI Avayogi Planet  : %s\n", dasha_lords[ay_ni_nak_idx % 9]);
        printf("-----------------------------------------------------------------\n");
    }
	
	void calculate_muhurat(int t_year, int t_month, int t_day, bool print_output) {
        double sunrise_ut, sunset_ut; double geopos[3] = {location.lon, location.lat, 0.0}; char serr[256];
        double local_midnight_ut = swe_julday(t_year, t_month, t_day, 0.0 - location.tz_offset, SE_GREG_CAL);
        swe_rise_trans(local_midnight_ut, SE_SUN, NULL, iflag, 769, geopos, 0, 0, &sunrise_ut, serr);
        swe_rise_trans(sunrise_ut + 0.01, SE_SUN, NULL, iflag, 770, geopos, 0, 0, &sunset_ut, serr);
        
        int y, m, d; double jut;
        swe_revjul(sunrise_ut + (location.tz_offset / 24.0), SE_GREG_CAL, &y, &m, &d, &jut); sunrise_hour_decimal = jut;
        swe_revjul(sunset_ut + (location.tz_offset / 24.0), SE_GREG_CAL, &y, &m, &d, &jut); sunset_hour_decimal = jut;
        
        double noon_jd = swe_julday(t_year, t_month, t_day, 12.0, SE_GREG_CAL);
        int calc_weekday = (int)(floor(noon_jd + 1.5)) % 7; 
        if (!print_output) { current_weekday = calc_weekday; }

        if (print_output && !json_mode) {
            double daytime = sunset_ut - sunrise_ut; double k_len = daytime / 8.0; double m_len = daytime / 15.0;
            int rahu_idx[] = {7, 1, 6, 4, 5, 3, 2}; int yama_idx[] = {4, 3, 2, 1, 0, 6, 5};   
            int guli_idx[] = {6, 5, 4, 3, 2, 1, 0}; int kulika_idx[] = {13, 11, 9, 7, 5, 3, 1};
            int kantaka_idx[] = {5, 3, 1, 13, 11, 9, 7}; int kalavela_idx[] = {7, 5, 3, 1, 13, 11, 9}; 
            int yamaghanta_idx[] = {9, 7, 5, 3, 1, 13, 11};
            
            // THE FIX: Adjusted Dushta Muhurtas array to flawlessly match classical 15-part daily indices
            vector<int> dushta_idx[7] = {{13}, {8, 11}, {3}, {7}, {5, 11}, {3, 8}, {0, 1}};

            printf("\n=== AUSPICIOUS & INAUSPICIOUS TIMINGS (MUHURAT) ===\n");
            printf("Target Date: %02d/%02d/%04d (%s)\n", t_day, t_month, t_year, weekdays[calc_weekday]);
            printf("--------------------------------------------------------------------------------\n");
            printf("Sunrise            : %s\n", format_time_only(sunrise_ut).c_str());
            printf("Sunset             : %s\n", format_time_only(sunset_ut).c_str());
            printf("--------------------------------------------------------------------------------\n");
            printf("[Inauspicious Timings - Ashubha Muhurat]\n");
            printf("Rahu Kaal          : %s to %s\n", format_time_only(sunrise_ut + rahu_idx[calc_weekday]*k_len).c_str(), format_time_only(sunrise_ut + (rahu_idx[calc_weekday]+1)*k_len).c_str());
            printf("Yamaganda          : %s to %s\n", format_time_only(sunrise_ut + yama_idx[calc_weekday]*k_len).c_str(), format_time_only(sunrise_ut + (yama_idx[calc_weekday]+1)*k_len).c_str());
            printf("Gulika Kaal        : %s to %s\n", format_time_only(sunrise_ut + guli_idx[calc_weekday]*k_len).c_str(), format_time_only(sunrise_ut + (guli_idx[calc_weekday]+1)*k_len).c_str());
            
            printf("Dushta Muhurtas    : ");
            for (size_t i = 0; i < dushta_idx[calc_weekday].size(); i++) {
                printf("%s to %s", format_time_only(sunrise_ut + dushta_idx[calc_weekday][i]*m_len).c_str(), format_time_only(sunrise_ut + (dushta_idx[calc_weekday][i]+1)*m_len).c_str());
                if (i < dushta_idx[calc_weekday].size() - 1) printf(", ");
            }
            printf("\n");
            
            printf("Kulika             : %s to %s\n", format_time_only(sunrise_ut + kulika_idx[calc_weekday]*m_len).c_str(), format_time_only(sunrise_ut + (kulika_idx[calc_weekday]+1)*m_len).c_str());
            printf("Kantaka / Mrityu   : %s to %s\n", format_time_only(sunrise_ut + kantaka_idx[calc_weekday]*m_len).c_str(), format_time_only(sunrise_ut + (kantaka_idx[calc_weekday]+1)*m_len).c_str());
            printf("Kalavela/Ardhayaam : %s to %s\n", format_time_only(sunrise_ut + kalavela_idx[calc_weekday]*m_len).c_str(), format_time_only(sunrise_ut + (kalavela_idx[calc_weekday]+1)*m_len).c_str());
            printf("Yamaghanta         : %s to %s\n", format_time_only(sunrise_ut + yamaghanta_idx[calc_weekday]*m_len).c_str(), format_time_only(sunrise_ut + (yamaghanta_idx[calc_weekday]+1)*m_len).c_str());
            
            printf("--------------------------------------------------------------------------------\n");
            printf("[Auspicious Timings - Shubha Muhurat]\n");
            printf("Abhijit Muhurat    : %s to %s\n", format_time_only(sunrise_ut + 7*m_len).c_str(), format_time_only(sunrise_ut + 8*m_len).c_str());
            printf("--------------------------------------------------------------------------------\n");
        }
    }
	
	void calculate_event_muhurat(string event_type, int target_year, int target_month) {
        string e_lower = event_type;
        transform(e_lower.begin(), e_lower.end(), e_lower.begin(), ::tolower);

        vector<int> valid_naks, valid_tithis, valid_days;
        string e_name_en, e_name_te;

        if (e_lower == "marriage" || e_lower == "vivaha") {
            e_name_en = "Marriage / Vivaha"; e_name_te = "వివాహం (మ్యారేజ్)";
            valid_naks = {3, 4, 9, 11, 12, 14, 16, 18, 20, 25, 26}; // Rohini, Mrig, Magha, U.Phal, Hasta, Swati, Anu, Mula, U.Ash, U.Bha, Revati
            valid_tithis = {1, 2, 4, 6, 9, 10, 12}; // Dwitiya, Tritiya, Panchami, Saptami, Dashami, Ekadashi, Trayodashi
            valid_days = {1, 3, 4, 5}; // Mon, Wed, Thu, Fri
        } 
        else if (e_lower == "house" || e_lower == "griha") {
            e_name_en = "House Warming / Griha Pravesh"; e_name_te = "గృహ ప్రవేశం";
            valid_naks = {3, 4, 11, 13, 16, 20, 25, 26}; 
            valid_tithis = {1, 2, 4, 6, 9, 10, 12}; 
            valid_days = {1, 3, 4, 5}; 
        }
        else if (e_lower == "vehicle" || e_lower == "vahana") {
            e_name_en = "Vehicle Purchase"; e_name_te = "వాహన కొనుగోలు (వెహికల్)";
            valid_naks = {0, 6, 7, 12, 14, 21, 22, 23, 26}; // Ashwini, Punarvasu, Pushya, Hasta, Swati, Shravana, Dhanishtha, Shatabhisha, Revati
            valid_tithis = {2, 3, 4, 6, 9, 10, 12, 14}; 
            valid_days = {0, 1, 3, 4, 5}; 
        }
        else if (e_lower == "electronics" || e_lower == "tv") {
            e_name_en = "Electronics / Appliances"; e_name_te = "ఎలక్ట్రానిక్స్ & ఉపకరణాలు (TV, Fridge)";
            valid_naks = {0, 4, 6, 7, 12, 14, 21, 22, 23};
            valid_tithis = {4, 6, 9, 10, 12, 14};
            valid_days = {0, 1, 3, 4, 5}; 
        }
        else if (e_lower == "gold" || e_lower == "jewelry") {
            e_name_en = "Gold / Jewelry Purchase"; e_name_te = "బంగారం / ఆభరణాల కొనుగోలు";
            valid_naks = {0, 3, 4, 7, 12, 13, 14, 16, 22, 23, 26};
            valid_tithis = {0, 1, 2, 4, 6, 9, 10, 12}; 
            valid_days = {0, 1, 3, 4, 5}; 
        }
        else {
            printf("Error: Event type not recognized. Valid options: marriage, house, vehicle, electronics, gold.\n");
            return;
        }

        if (telugu_mode) {
            printf("\n===============================================================================================================\n");
            printf("=== వ్యక్తిగత ముహూర్త శోధన (PERSONALIZED MUHURAT SCANNER) ===\n");
            printf("కార్యక్రమం  : %s\n", e_name_te.c_str());
            printf("శోధన నెల   : %02d/%04d\n", target_month, target_year);
            printf("జన్మ నక్షత్రం : %s (తారాబలం ఫిల్టర్ చేయబడింది)\n", get_nak_name((int)(moon_lon / (360.0 / 27.0))).c_str());
            printf("===============================================================================================================\n");
            printf("%-12s | %-12s | %-20s | %-15s | %-25s | %-15s\n", "తేదీ", "వారం", "తిథి", "నక్షత్రం", "తారాబలం", "శుభ సమయం");
        } else {
            printf("\n===============================================================================================================\n");
            printf("=== PERSONALIZED EVENT MUHURAT SCANNER ===\n");
            printf("Event Type  : %s\n", e_name_en.c_str());
            printf("Search Month: %02d/%04d\n", target_month, target_year);
            printf("Natal Star  : %s (Tara Bala Filter Applied)\n", nak_names[(int)(moon_lon / (360.0 / 27.0))]);
            printf("===============================================================================================================\n");
            printf("%-12s | %-12s | %-20s | %-15s | %-25s | %-15s\n", "Date", "Weekday", "Tithi", "Nakshatra", "Tara Bala", "Best Window");
        }
        printf("---------------------------------------------------------------------------------------------------------------\n");

        double start_jd = swe_julday(target_year, target_month, 1, 0.0 - location.tz_offset, SE_GREG_CAL);
        int valid_hits = 0;
        int natal_mo_nak = (int)(moon_lon / (360.0 / 27.0)); // User's Birth Star!

        // Sweep every day of the month
        for (int i = 0; i < 31; i++) {
            double noon_jd = start_jd + i + (12.0 / 24.0); 
            
            int y, m, d; double jut;
            swe_revjul(noon_jd + (location.tz_offset / 24.0), SE_GREG_CAL, &y, &m, &d, &jut);
            if (m != target_month) break; 

            int weekday = (int)(floor(noon_jd + 1.5)) % 7; 
            
            double xx_sun[6], xx_moon[6]; char serr[256];
            swe_calc_ut(noon_jd, SE_SUN, iflag, xx_sun, serr); 
            swe_calc_ut(noon_jd, SE_MOON, iflag, xx_moon, serr);
            
            double t_angle = fmod((xx_moon[0] - xx_sun[0] + 360.0), 360.0);
            int t_idx = (int)(t_angle / 12.0); 
            int tithi_15_scale = t_idx % 15; 
            int n_idx = (int)(xx_moon[0] / (360.0 / 27.0));

            // --- PERSONALIZED TARA BALA CHECK ---
            int tara_idx = (n_idx - natal_mo_nak + 27) % 9;
            // Reject Vipat (2), Pratyak (4), and Vadha (6) Taras automatically!
            bool tara_ok = (tara_idx != 2 && tara_idx != 4 && tara_idx != 6);

            bool day_ok = std::find(valid_days.begin(), valid_days.end(), weekday) != valid_days.end();
            bool nak_ok = std::find(valid_naks.begin(), valid_naks.end(), n_idx) != valid_naks.end();
            bool tithi_ok = std::find(valid_tithis.begin(), valid_tithis.end(), tithi_15_scale) != valid_tithis.end();

            if (day_ok && nak_ok && tithi_ok && tara_ok) {
                double sunrise_ut, sunset_ut; double geopos[3] = {location.lon, location.lat, 0.0};
                swe_rise_trans(start_jd + i, SE_SUN, NULL, iflag, 769, geopos, 0, 0, &sunrise_ut, serr);
                swe_rise_trans(sunrise_ut + 0.01, SE_SUN, NULL, iflag, 770, geopos, 0, 0, &sunset_ut, serr);
                
                double daytime = sunset_ut - sunrise_ut; 
                double m_len = daytime / 15.0;
                double abhijit_start = sunrise_ut + 7*m_len;
                double abhijit_end = sunrise_ut + 8*m_len;
                
                char date_buf[32]; snprintf(date_buf, sizeof(date_buf), "%02d/%02d/%04d", d, m, y);
                string ab_window = format_time_only(abhijit_start).substr(0, 5) + " - " + format_time_only(abhijit_end).substr(0, 5);
                string full_tithi = get_tithi(t_idx) + " (" + get_paksha(t_idx).substr(0,3) + ")";
                
                string tara_print = get_tara(tara_idx);
                // Strip description in English for neatness in table
                if (!telugu_mode) tara_print = tara_print.substr(0, tara_print.find(" ("));

                printf("%-12s | %-12s | %-20s | %-15s | %-25s | %-15s\n", 
                       date_buf, get_weekday(weekday).c_str(), full_tithi.c_str(), get_nak_name(n_idx).c_str(), tara_print.c_str(), ab_window.c_str());
                valid_hits++;
            }
        }
        
        if (valid_hits == 0) {
            if (telugu_mode) printf("మీ జన్మ నక్షత్రం ఆధారంగా ఈ మాసంలో %s కొరకు ఎటువంటి అనుకూల ముహూర్తాలు లేవు.\n", e_name_te.c_str());
            else printf("Based on your Natal Star, no personalized safe Panchang alignments were found for %s in this month.\n", e_name_en.c_str());
        }
        printf("===============================================================================================================\n");
    }
	
	// =========================================================================
    // DAILY PANCHANG & TIMINGS (RESTORED & ENHANCED)
    // =========================================================================

	void calculate_daily_panchang_transitions(int t_year, int t_month, int t_day) {
        if (json_mode) return;
        double local_midnight_ut = swe_julday(t_year, t_month, t_day, 0.0 - location.tz_offset, SE_GREG_CAL);
        double noon_jd = swe_julday(t_year, t_month, t_day, 12.0, SE_GREG_CAL);
        int calc_weekday = (int)(floor(noon_jd + 1.5)) % 7; 
        
        if (telugu_mode) {
            printf("\n=== రోజువారీ పంచాంగం (తిథి & నక్షత్ర సమయాలు) ===\n");
            printf("వారం      : %s\n", get_weekday(calc_weekday).c_str());
        } else {
            printf("\n=== DAILY PANCHANG (TITHI & NAKSHATRA EXACT TIMINGS) ===\n");
            printf("Day       : %s\n", weekdays[calc_weekday]);
        }
        printf("------------------------------------------------------------------------------------------\n");
        
        int current_tithi = -1, current_nak = -1;

        for (int i = 0; i <= 86400; i += 60) {
            double jd = local_midnight_ut + (i / 86400.0);
            double xx_sun[6], xx_moon[6]; char serr[256];
            swe_calc_ut(jd, SE_SUN, iflag, xx_sun, serr); 
            swe_calc_ut(jd, SE_MOON, iflag, xx_moon, serr);
            
            double t_angle = fmod((xx_moon[0] - xx_sun[0] + 360.0), 360.0);
            int t_idx = (int)(t_angle / 12.0); 
            int n_idx = (int)(xx_moon[0] / (360.0 / 27.0));
            
            if (i == 0) {
                current_tithi = t_idx; current_nak = n_idx;
                if (telugu_mode) {
                    printf("తిథి      : %s (%s) అర్ధరాత్రి ప్రారంభం\n", get_tithi(t_idx).c_str(), get_paksha(t_idx).c_str());
                    printf("నక్షత్రం  : %s అర్ధరాత్రి ప్రారంభం\n", get_nak_name(n_idx).c_str());
                } else {
                    printf("Tithi     : %s (%s) active at Midnight\n", get_tithi(t_idx).c_str(), get_paksha(t_idx).c_str());
                    printf("Nakshatra : %s active at Midnight\n", get_nak_name(n_idx).c_str());
                }
            } else {
                if (t_idx != current_tithi) {
                    if (telugu_mode) printf("తిథి      : %s ముగుస్తుంది, %s ప్రారంభం: %s కు\n", get_tithi(current_tithi).c_str(), get_tithi(t_idx).c_str(), format_time_only(jd).c_str());
                    else printf("Tithi     : %s ends, %s begins at %s\n", get_tithi(current_tithi).c_str(), get_tithi(t_idx).c_str(), format_time_only(jd).c_str());
                    current_tithi = t_idx;
                }
                if (n_idx != current_nak) {
                    if (telugu_mode) printf("నక్షత్రం  : %s ముగుస్తుంది, %s ప్రారంభం: %s కు\n", get_nak_name(current_nak).c_str(), get_nak_name(n_idx).c_str(), format_time_only(jd).c_str());
                    else printf("Nakshatra : %s ends, %s begins at %s\n", get_nak_name(current_nak).c_str(), get_nak_name(n_idx).c_str(), format_time_only(jd).c_str());
                    current_nak = n_idx;
                }
            }
        }
        printf("------------------------------------------------------------------------------------------\n");
    }
	
	void calculate_daily_lagnas(int year, int month, int day) {
        printf("\n=== DAILY LAGNA & NAKSHATRA PADA TRANSITIONS ===\n");
        printf("------------------------------------------------------------------------------------------\n");
        double jd_start = swe_julday(year, month, day, 0.0 - location.tz_offset, SE_GREG_CAL);
        int current_rashi = -1, current_pada_abs = -1; 
        vector<Transition> transitions;

        for (int i = 0; i <= 86400; i += 60) { 
            double jd = jd_start + (i / 86400.0); double cusps[13], ascmc[10];
            swe_houses_ex(jd, iflag, location.lat, location.lon, 'P', cusps, ascmc);
            double lagna = ascmc[0];
            
            int rashi_idx = (int)(lagna / 30.0);
            int pada_abs = (int)(lagna / ((360.0/27.0)/4.0)); // Tracking exact Pada boundary

            if (rashi_idx != current_rashi || pada_abs != current_pada_abs) {
                int y, mo, d; double jut; swe_revjul(jd + (location.tz_offset / 24.0), SE_GREG_CAL, &y, &mo, &d, &jut);
                int h = (int)jut; int m = (int)((jut - h) * 60.0); int s = (int)round((((jut - h) * 60.0) - m) * 60.0);
                if (s >= 60) { s -= 60; m += 1; } if (m >= 60) { m -= 60; h += 1; } if (h >= 24) { h -= 24; }
                
                int nak_idx = (int)(lagna / (360.0 / 27.0));
                int pada = (int)((lagna - (nak_idx * (360.0 / 27.0))) / ((360.0 / 27.0) / 4.0)) + 1;
                
                transitions.push_back({rashi_idx, nak_idx, pada, h, m, s, (rashi_idx != current_rashi)});
                current_rashi = rashi_idx; current_pada_abs = pada_abs;
            }
        }
        
        printf("%-15s | %-20s | %-10s | %-10s\n", "Lagna", "Nakshatra-Pada", "Begins", "Ends");
        printf("------------------------------------------------------------------------------------------\n");
        for (size_t i = 0; i < transitions.size(); i++) {
            char ends[20];
            if (i + 1 < transitions.size()) snprintf(ends, sizeof(ends), "%02d:%02d:%02d", transitions[i+1].h, transitions[i+1].m, transitions[i+1].s);
            else snprintf(ends, sizeof(ends), "23:59:59");
            
            char nak_pada[32];
            snprintf(nak_pada, sizeof(nak_pada), "%s %d", nak_names[transitions[i].nak_idx], transitions[i].pada);
            printf("%-15s | %-20s | %02d:%02d:%02d   | %-10s\n", rashi_names[transitions[i].rashi_idx], nak_pada, transitions[i].h, transitions[i].m, transitions[i].s, ends);
        }
    }
	// =========================================================================
    // KRISHNAMURTI PADDHATI (KP) AUTOMATION
    // =========================================================================

    void calculate_kp() {
        if (json_mode) return;
        double nak_size = 360.0 / 27.0;
        
        auto get_sublord = [&](double lon) {
            int n_idx = (int)(lon / nak_size); int l_idx = n_idx % 9;
            double p_deg = lon - (n_idx * nak_size); double c_pos = 0.0;
            for (int i = 0; i < 9; i++) {
                int sl_idx = (l_idx + i) % 9; double sl_size = (dasha_years[sl_idx] / 120.0) * nak_size;
                c_pos += sl_size; if (p_deg < c_pos) return sl_idx;
            } return l_idx;
        };

        // Map Dasha array indices (Ke, Ve, Su...) to our standard planet indices (1=Sun, 2=Moon...)
        int dasha_to_planet_idx[] = {9, 6, 1, 2, 3, 8, 5, 7, 4}; 

        auto get_houses_string = [&](int p_idx) {
            int placed_h = (planet_rashis[p_idx] - planet_rashis[0] + 12) % 12 + 1;
            string res = to_string(placed_h);
            
            if (p_idx >= 1 && p_idx <= 7) { // Exclude Rahu/Ketu from standard lordship
                for (int h = 1; h <= 12; h++) {
                    int rashi_of_house = (planet_rashis[0] + h - 1) % 12;
                    if (rashi_lords[rashi_of_house] == string(p_names_full[p_idx])) {
                        if (h != placed_h) res += "," + to_string(h);
                    }
                }
            }
            return res;
        };

        printf("\n=== KRISHNAMURTI PADDHATI (KP) PLANETARY DETAILS ===\n");
        printf("--------------------------------------------------------------------------------\n");
        printf("%-9s | %-15s | %-10s | %-10s | %-10s\n", "Graha", "Longitude", "Rasi Lord", "Star Lord", "Sub Lord");
        printf("--------------------------------------------------------------------------------\n");
        for (int i = 0; i < 10; i++) {
            double lon = planet_lons[i]; int r_lord = (int)(lon / 30.0);
            int n_lord = ((int)(lon / nak_size)) % 9; int s_lord = get_sublord(lon);
            printf("%-9s | %-15s | %-10s | %-10s | %-10s\n", p_names_full[i], format_dms(lon).c_str(), rashi_lords[r_lord], dasha_lords[n_lord], dasha_lords[s_lord]);
        }
        
        printf("\n[KP SYNTHESIS: STAR LORD RESULTS & SUB LORD OUTCOMES]\n");
        for (int i = 1; i <= 9; i++) {
            double lon = planet_lons[i];
            int n_lord = ((int)(lon / nak_size)) % 9; 
            int s_lord = get_sublord(lon);
            
            int sl_p_idx = dasha_to_planet_idx[n_lord];
            int sub_p_idx = dasha_to_planet_idx[s_lord];
            
            printf("  - %s:\n", p_names_full[i]);
            printf("    * Offers the results of Star Lord %s (Signifying Houses: %s)\n", p_names_full[sl_p_idx], get_houses_string(sl_p_idx).c_str());
            printf("    * Final Outcome decided by Sub Lord %s (Signifying Houses: %s)\n", p_names_full[sub_p_idx], get_houses_string(sub_p_idx).c_str());
        }
        printf("--------------------------------------------------------------------------------\n");
    }

    void calculate_daily_horas(int t_year, int t_month, int t_day) {
        if (json_mode) return;
        double sunrise_ut, sunset_ut, next_sunrise_ut; double geopos[3] = {location.lon, location.lat, 0.0}; char serr[256];
        double local_midnight_ut = swe_julday(t_year, t_month, t_day, 0.0 - location.tz_offset, SE_GREG_CAL);
        swe_rise_trans(local_midnight_ut, SE_SUN, NULL, iflag, 769, geopos, 0, 0, &sunrise_ut, serr);
        swe_rise_trans(sunrise_ut + 0.01, SE_SUN, NULL, iflag, 770, geopos, 0, 0, &sunset_ut, serr);
        swe_rise_trans(sunset_ut + 0.01, SE_SUN, NULL, iflag, 769, geopos, 0, 0, &next_sunrise_ut, serr);

        int weekday = (int)(floor(swe_julday(t_year, t_month, t_day, 12.0, SE_GREG_CAL) + 1.5)) % 7; 
        int hora_seq[] = {0, 5, 3, 1, 6, 4, 2}; int day_lord_idx = 0;
        for (int i=0; i<7; i++) { if (hora_seq[i] == weekday) day_lord_idx = i; }
        const char* p_names[] = {"Surya", "Chandra", "Mangal", "Budha", "Guru", "Shukra", "Shani"};

        double day_hora = (sunset_ut - sunrise_ut) / 12.0;
        double night_hora = (next_sunrise_ut - sunset_ut) / 12.0;

        printf("\n=== 24 PLANETARY HORAS ===\n");
        printf("-----------------------------------------------------------------\n");
        printf("%-5s | %-10s | %-10s | %-10s\n", "Hora", "Lord", "Starts", "Ends");
        printf("-----------------------------------------------------------------\n");
        for (int i = 0; i < 24; i++) {
            int h_lord = hora_seq[(day_lord_idx + i) % 7];
            double h_start = (i < 12) ? sunrise_ut + (i * day_hora) : sunset_ut + ((i - 12) * night_hora);
            double h_end = (i < 12) ? sunrise_ut + ((i + 1) * day_hora) : sunset_ut + ((i - 11) * night_hora);
            char h_num[10]; snprintf(h_num, sizeof(h_num), "%d", i + 1);
            printf("%-5s | %-10s | %-10s | %-10s\n", h_num, p_names[h_lord], format_time_only(h_start).c_str(), format_time_only(h_end).c_str());
            if (i == 11) printf("--------------------- Sunset ------------------------------------\n");
        }
        printf("-----------------------------------------------------------------\n");
    }

    void calculate_varsha_masa(int& v_lord, int& m_lord) {
        char serr[256]; double xx[6]; double search_jd = tjd_ut - 365.0; double aries_ingress_jd = 0;
        for (int i = 0; i < 400; i++) {
            swe_calc_ut(search_jd + i, SE_SUN, iflag, xx, serr);
            if (xx[0] > 330.0 || xx[0] < 30.0) { 
                double prev_xx[6]; swe_calc_ut(search_jd + i - 1, SE_SUN, iflag, prev_xx, serr);
                if (prev_xx[0] > 345.0 && xx[0] < 15.0) { 
                    double low = search_jd + i - 1, high = search_jd + i;
                    for (int iter=0; iter<20; iter++) {
                        double mid = (low+high)/2.0; swe_calc_ut(mid, SE_SUN, iflag, xx, serr);
                        if (xx[0] > 180.0) low = mid; else high = mid; 
                    }
                    aries_ingress_jd = (low+high)/2.0; break;
                }
            }
        }
        if (aries_ingress_jd > 0) {
            double local_ingress = aries_ingress_jd + (location.tz_offset / 24.0);
            int v_day = (int)(floor(local_ingress + 1.5)) % 7;
            int day_to_planet[] = {0, 1, 2, 3, 4, 5, 6}; v_lord = day_to_planet[v_day];

            int current_solar_month = (int)(sun_lon / 30.0); double target_deg = current_solar_month * 30.0;
            double m_low = tjd_ut - 35.0, m_high = tjd_ut;
            for (int iter=0; iter<30; iter++) {
                 double mid = (m_low+m_high)/2.0; swe_calc_ut(mid, SE_SUN, iflag, xx, serr);
                 double dist = xx[0] - target_deg; if (dist < 0) dist += 360.0;
                 if (dist > 180.0) m_low = mid; else m_high = mid;
            }
            double month_ingress_jd = (m_low+m_high)/2.0;
            double local_month_ingress = month_ingress_jd + (location.tz_offset / 24.0);
            int m_day = (int)(floor(local_month_ingress + 1.5)) % 7; m_lord = day_to_planet[m_day];
        }
    }

    void calculate_aspects() {
        if (json_mode) return;
        const char* s_names[] = {"Su", "Mo", "Ma", "Me", "Ju", "Ve", "Sa", "Ra", "Ke", "Asc"};
        printf("\n=== PLANETARY ASPECTS (RASHI DRISHTI) ===\n");
        printf("--------------------------------------------------------------------------------\n");
        printf("%-8s | %-5s | %-25s | %-30s\n", "Graha", "Pos", "Aspects Signs", "Aspects Planets/Asc");
        printf("--------------------------------------------------------------------------------\n");
        for (int i = 1; i <= 9; i++) {
            vector<int> a_rashis; int r = planet_rashis[i];
            a_rashis.push_back((r + 6) % 12);
            if (i == 3) { a_rashis.push_back((r + 3) % 12); a_rashis.push_back((r + 7) % 12); } 
            else if (i == 5 || i == 8 || i == 9) { a_rashis.push_back((r + 4) % 12); a_rashis.push_back((r + 8) % 12); } 
            else if (i == 7) { a_rashis.push_back((r + 2) % 12); a_rashis.push_back((r + 9) % 12); }
            sort(a_rashis.begin(), a_rashis.end());
            string s_str = "", p_str = "";
            for (size_t j = 0; j < a_rashis.size(); j++) {
                s_str += short_rashi[a_rashis[j]]; if (j < a_rashis.size() - 1) s_str += ", ";
                for (int p = 0; p < 10; p++) { if (p != i && planet_rashis[p] == a_rashis[j]) { p_str += s_names[p]; p_str += " "; } }
            }
            if (p_str == "") p_str = "-";
            printf("%-8s | %-5s | %-25s | %-30s\n", p_names_full[i], short_rashi[r], s_str.c_str(), p_str.c_str());
        }
        printf("--------------------------------------------------------------------------------\n");
    }

    void calculate_shodashvarga() {
        if (json_mode) return;
        int vargas[] = {1, 2, 3, 4, 7, 9, 10, 11, 12, 16, 20, 24, 27, 30, 40, 45, 60};
        printf("\n=== DIVISIONAL CHARTS (INCLUDING D11 RUDRAMSHA) ===\n");
        printf("----------------------------------------------------------------------------------------------------------------------------\n");
        printf("%-7s | D1  | D2  | D3  | D4  | D7  | D9  | D10 | D11 | D12 | D16 | D20 | D24 | D27 | D30 | D40 | D45 | D60\n", "Graha");
        printf("----------------------------------------------------------------------------------------------------------------------------\n");
        for(int p = 0; p < 10; p++) {
            printf("%-7s | ", p_names_full[p]);
            for(int v = 0; v < 17; v++) {
                int sign_idx = get_varga(vargas[v], planet_lons[p]);
                printf("%-3s | ", short_rashi[sign_idx]);
            }
            printf("\n");
        }
        printf("----------------------------------------------------------------------------------------------------------------------------\n");
    }

	void calculate_ashtakavarga(bool silent = false) {
        if (json_mode) return;
        const string av_masks[7][8] = {
            // Sun (Total: 48)
            {"110100111110", "001001000110", "110100111110", "001011001111", "000011001010", "000001100001", "110100111110", "001101000111"},
            // Moon (Total: 49)
            {"001001110110", "111001100110", "011011001110", "101110110110", "100100110111", "001110101110", "001011000010", "001001000110"},
            // Mars (Total: 39 - Fixed Sat->Ma mask)
            {"001011000110", "001001000010", "110100110110", "001011000010", "000001000111", "000001010011", "100100111110", "101001000110"},
            // Mercury (Total: 54)
            {"000011001011", "010101010110", "110100111110", "101011001111", "000001010011", "111110011010", "110100111110", "110101010110"},
            // Jupiter (Total: 56 - Fixed Asc->Ju mask)
            {"111100111110", "010010101010", "110100110110", "110111001110", "111100110110", "010011001110", "001011000001", "110111101110"},
            // Venus (Total: 52)
            {"000000010011", "111110011011", "001011001011", "001011001010", "000010011110", "111110011110", "001110011110", "111110011010"},
            // Saturn (Total: 39 - Fixed Ma->Sa mask)
            {"110100110110", "001001000010", "001011000111", "000001011111", "000011000011", "000001000011", "001011000010", "101101000110"}
        };
        
        memset(sav_scores, 0, sizeof(sav_scores));
        memset(bav_scores, 0, sizeof(bav_scores));

        for (int target = 0; target < 7; target++) {
            for (int source = 0; source < 8; source++) {
                
                // CRITICAL FIX: Map the AV array index (0=Sun...7=Asc) to our planet_rashis array index (0=Asc, 1=Sun...)
                int pr_idx = (source == 7) ? 0 : source + 1;
                int source_rashi = planet_rashis[pr_idx]; 
                
                for (int house_offset = 0; house_offset < 12; house_offset++) {
                    if (av_masks[target][source][house_offset] == '1') {
                        int dest_rashi = (source_rashi + house_offset) % 12;
                        bav_scores[target][dest_rashi]++; 
                        sav_scores[dest_rashi]++;
                    }
                }
            }
        }
        av_calculated = true;

        if (!silent) {
            printf("\n=== ASHTAKAVARGA (BAV & SAV TOTALS) ===\n");
            printf("----------------------------------------------------------------------\n");
            printf("%-10s | %-2s | %-2s | %-2s | %-2s | %-2s | %-2s | %-2s | %-9s\n", "Rashi", "Su", "Mo", "Ma", "Me", "Ju", "Ve", "Sa", "SAV Total");
            printf("----------------------------------------------------------------------\n");
            for (int r = 0; r < 12; r++) {
                printf("%-10s | %2d | %2d | %2d | %2d | %2d | %2d | %2d |    %3d\n", rashi_names[r], bav_scores[0][r], bav_scores[1][r], bav_scores[2][r], bav_scores[3][r], bav_scores[4][r], bav_scores[5][r], bav_scores[6][r], sav_scores[r]);
            }
            printf("----------------------------------------------------------------------\n");
        }
    }

    void calculate_panchang() {
        if (json_mode) return;
        double tithi_angle = fmod((moon_lon - sun_lon + 360.0), 360.0);
        int tithi_index = (int)(tithi_angle / 12.0); 
        string paksha = (tithi_index < 15) ? "Shukla Paksha" : "Krishna Paksha";
        double yoga_angle = fmod((moon_lon + sun_lon), 360.0);
        int yoga_index = (int)(yoga_angle / (10.0 / 3.0 * 4.0)); 
        printf("\n=== GEOMETRIC PANCHANG ===\n");
        printf("Tithi     : %s (%s)\n", tithi_names[tithi_index], paksha.c_str());
        printf("Yoga      : %s\n", yoga_names[yoga_index]);
        printf("----------------------------------------------------------------------------------------------------\n");
    }

    void calculate_dasha_balance() {
        if (json_mode) return;
        double nak_size = 360.0 / 27.0; 
        int n_idx = (int)(moon_lon / nak_size);
        int l_idx = n_idx % 9;
        double frac = (moon_lon - (n_idx * nak_size)) / nak_size;
        double left = (1.0 - frac) * dasha_years[l_idx];
        int y = (int)left; double m_rem = (left - y) * 12.0; int m = (int)m_rem;
        int d = (int)round((m_rem - m) * (365.2425 / 12.0)); 
        printf("\n=== VIMSHOTTARI DASHA BALANCE ===\n");
        printf("Maha Dasha: %s (Balance: %d Years, %d Months, %d Days)\n", dasha_lords[l_idx], y, m, d);
    }

    void calculate_6_level_dasha_target(int t_year, int t_month, int t_day, int t_hour, int t_min, int t_sec, bool is_current_clock = false) {
        if (json_mode) return;
        double target_jd;
        if (is_current_clock) {
            time_t t = time(nullptr); tm* now_utc = gmtime(&t);
            double ut_dec = now_utc->tm_hour + (now_utc->tm_min / 60.0) + (now_utc->tm_sec / 3600.0);
            target_jd = swe_julday(now_utc->tm_year + 1900, now_utc->tm_mon + 1, now_utc->tm_mday, ut_dec, SE_GREG_CAL);
            int y, m, d; double jut; swe_revjul(target_jd + (location.tz_offset / 24.0), SE_GREG_CAL, &y, &m, &d, &jut);
            int print_h = (int)jut; int print_m = (int)((jut - print_h) * 60.0);
            printf("\n=== 6-LAYER DASHA STATUS FOR TARGET DATE: %02d/%02d/%04d %02d:%02d (CURRENT CLOCK) ===\n", d, m, y, print_h, print_m);
        } else {
            double ut_dec = t_hour + (t_min / 60.0) + (t_sec / 3600.0) - location.tz_offset;
            target_jd = swe_julday(t_year, t_month, t_day, ut_dec, SE_GREG_CAL); 
            printf("\n=== 6-LAYER DASHA STATUS FOR TARGET DATE: %02d/%02d/%04d %02d:%02d:%02d ===\n", t_day, t_month, t_year, t_hour, t_min, t_sec);
        }
        
        double nak_size = 360.0 / 27.0; 
        int nak_index = (int)(moon_lon / nak_size);
        int lord_index = nak_index % 9;
        double fraction_passed = (moon_lon - (nak_index * nak_size)) / nak_size;
        double life_start_jd = tjd_ut - (fraction_passed * dasha_years[lord_index] * 365.2425);
        
        printf("----------------------------------------------------------------------------------------\n");
        printf("%-20s | %-10s | %-19s | %-19s\n", "Level", "Lord", "Starts", "Ends");
        printf("----------------------------------------------------------------------------------------\n");

        double current_start_jd = life_start_jd; double current_duration = 120.0 * 365.2425; int current_lord = lord_index;

        for (int level = 0; level < 6; level++) {
            double loop_start_jd = current_start_jd; int active_lord = -1;
            double active_start = 0, active_end = 0, active_duration = 0;

            for (int i = 0; i < 9; i++) {
                int p_idx = (current_lord + i) % 9;
                double ratio = dasha_years[p_idx] / 120.0;
                double sub_duration = current_duration * ratio;
                double loop_end_jd = loop_start_jd + sub_duration;

                if (target_jd >= loop_start_jd && target_jd < loop_end_jd) {
                    active_lord = p_idx; active_start = loop_start_jd; active_end = loop_end_jd; active_duration = sub_duration; break;
                }
                loop_start_jd = loop_end_jd;
            }
            if (active_lord != -1) {
                printf("%-20s | %-10s | %-19s | %-19s\n", dasha_levels[level], dasha_lords[active_lord], jd_to_string(active_start).c_str(), jd_to_string(active_end).c_str());
                current_start_jd = active_start; current_duration = active_duration; current_lord = active_lord; 
            } else { break; }
        }
        printf("----------------------------------------------------------------------------------------\n");
    }

    void interactive_dasha() {
        double nak_size = 360.0 / 27.0; 
        int nak_index = (int)(moon_lon / nak_size); int lord_index = nak_index % 9;
        double fraction_passed = (moon_lon - (nak_index * nak_size)) / nak_size;
        double life_start_jd = tjd_ut - (fraction_passed * dasha_years[lord_index] * 365.2425);

        printf("\n=== INTERACTIVE DASHA EXPLORER ===\n");
        printf("Enter 1-9 to select a period. Enter 0 to go back.\n");

        vector<DashaState> history; history.push_back({life_start_jd, 120.0 * 365.2425, lord_index});
        int level = 0;
        
        while (level >= 0 && level < 6) {
            DashaState current = history.back();
            printf("\n--- %s Options ---\n", dasha_levels[level]);
            printf("------------------------------------------------------------------\n");
            printf("Opt | Lord       | Starts              | Ends\n");
            printf("------------------------------------------------------------------\n");

            vector<DashaPeriod> options; double loop_start_jd = current.start;

            for (int i = 0; i < 9; i++) {
                int p_idx = (current.lord + i) % 9;
                double ratio = dasha_years[p_idx] / 120.0;
                double sub_duration = current.duration * ratio;
                double loop_end_jd = loop_start_jd + sub_duration;
                options.push_back({p_idx, loop_start_jd, sub_duration});
                printf(" %d  | %-10s | %-19s | %-19s\n", i + 1, dasha_lords[p_idx], jd_to_string(loop_start_jd).c_str(), jd_to_string(loop_end_jd).c_str());
                loop_start_jd = loop_end_jd;
            }
            if (level == 5) printf("\nEnd of Deha Dasha depth reached. Enter 0 to go back, or 99 to exit.\n"); 
            
            int choice = -1;
            while (true) {
                printf("\nEnter option (1-9 to drill down, 0 to go back): ");
                if (!(cin >> choice)) { cin.clear(); cin.ignore(10000, '\n'); choice = -1; }
                if (level == 5 && choice == 99) return; 
                if (choice == 0) { if (level == 0) { printf("Exiting Explorer.\n"); return; } break; }
                if (choice >= 1 && choice <= 9 && level < 5) break;
                if (level == 5 && choice == 0) break;
            }

            if (choice == 0) { history.pop_back(); level--; } 
            else { history.push_back({options[choice - 1].start_jd, options[choice - 1].duration, options[choice - 1].lord_idx}); level++; }
        }
    }

	// Add this helper function to print the tree
    void dfs_print_hierarchy(int level, int max_level, int current_lord, double start_jd, double duration, vector<string> path) {
        if (level >= max_level) return;
        
        double loop_start = start_jd;
        string indent = "";
        for(int i = 0; i < level; i++) indent += "    "; // Add 4 spaces per depth level

        for (int i = 0; i < 9; i++) {
            int p_idx = (current_lord + i) % 9;
            double sub_dur = duration * (dasha_years[p_idx] / 120.0);
            
            printf("%s[%s] %-10s | Starts: %-19s | Ends: %-19s\n", 
                   indent.c_str(), dasha_levels[level], dasha_lords[p_idx], 
                   jd_to_string(loop_start).c_str(), jd_to_string(loop_start + sub_dur).c_str());

            vector<string> new_path = path; new_path.push_back(short_dasha[p_idx]);
            dfs_print_hierarchy(level + 1, max_level, p_idx, loop_start, sub_dur, new_path);
            loop_start += sub_dur;
        }
    }

void print_dasha_web() {
        double nak_size = 360.0 / 27.0; 
        int nak_index = (int)(moon_lon / nak_size); 
        int lord_index = nak_index % 9;
        double fraction_passed = (moon_lon - (nak_index * nak_size)) / nak_size;
        double life_start_jd = tjd_ut - (fraction_passed * dasha_years[lord_index] * 365.2425);

        if (telugu_mode) {
            printf("\n=========================================================================================\n");
            printf("=== జీవిత కాల దశలు (DYNAMIC VIMSHOTTARI DASHA PREDICTIONS) ===\n");
            printf("=========================================================================================\n\n");
        } else {
            printf("\n=========================================================================================\n");
            printf("=== LIFE CHAPTERS (DYNAMIC VIMSHOTTARI DASHA PREDICTIONS) ===\n");
            printf("=========================================================================================\n\n");
        }

        int d_map[] = {9, 6, 1, 2, 3, 8, 5, 7, 4}; // Maps dasha lord index to standard planet index

        double cur_start = life_start_jd;
        for (int i = 0; i < 9; i++) {
            int md_idx = (lord_index + i) % 9;
            int md_p = d_map[md_idx];
            double md_dur = 120.0 * 365.2425 * (dasha_years[md_idx] / 120.0);
            
            string start_date = jd_to_string(cur_start).substr(0, 10);
            string end_date = jd_to_string(cur_start + md_dur).substr(0, 10);
            
            int score = natal_scores[md_p];
            int house = (planet_rashis[md_p] - planet_rashis[0] + 12) % 12 + 1;

            if (telugu_mode) {
                printf("⭐ [ %s  నుండి  %s ] : %s మహాదశ (ఆధిపత్యం: %dవ భావం | బలం: %d)\n", start_date.c_str(), end_date.c_str(), get_planet_name(md_p).c_str(), house, score);
                printf("   %s\n\n", te_get_dynamic_mahadasha(md_p, score, house).c_str());
            } else {
                printf("⭐ [ %s  to  %s ] : %s MAHADASHA (Placement: House %d | Dignity Score: %d)\n", start_date.c_str(), end_date.c_str(), p_names_full[md_p], house, score);
                printf("   %s\n\n", get_dynamic_mahadasha(md_p, score, house).c_str());
            }

            double ad_start = cur_start;
            for (int j = 0; j < 9; j++) {
                int ad_idx = (md_idx + j) % 9;
                int ad_p = d_map[ad_idx];
                double ad_dur = md_dur * (dasha_years[ad_idx] / 120.0);
                
                string ad_start_str = jd_to_string(ad_start).substr(0, 10);
                string ad_end_str = jd_to_string(ad_start + ad_dur).substr(0, 10);
                
                // --- NEW: Calculate Dignity and House for the Bhukti Planet ---
                int ad_score = natal_scores[ad_p];
                int ad_house = (planet_rashis[ad_p] - planet_rashis[0] + 12) % 12 + 1;

                // --- NEW: Calculate House Ownerships for Life-Event Injection ---
                vector<int> owned_houses;
                if (ad_p >= 1 && ad_p <= 7) { // Only standard physical planets rule houses
                    for (int h = 1; h <= 12; h++) {
                        int rashi_of_house = (planet_rashis[0] + h - 1) % 12;
                        if (rashi_lords[rashi_of_house] == string(p_names_full[ad_p])) {
                            owned_houses.push_back(h);
                        }
                    }
                }

                if (telugu_mode) {
                    printf("     -> [ %s - %s ] : %s భుక్తి\n", ad_start_str.c_str(), ad_end_str.c_str(), get_planet_name(ad_p).c_str());
                    printf("        %s\n", te_get_dynamic_bhukti(md_p, ad_p, ad_score, ad_house).c_str());
                    
                    // Inject the direct Life-Event prediction
                    if (ad_p == 8 || ad_p == 9) {
                        printf("        * ప్రత్యక్ష సంఘటనలు: %s\n", te_get_node_bhukti_event(get_planet_name(ad_p), ad_house).c_str());
                    } else {
                        printf("        * ప్రత్యక్ష సంఘటనలు: %s\n", te_get_lordship_bhukti_event(get_planet_name(ad_p), owned_houses).c_str());
                    }
                } else {
                    printf("     -> [ %s - %s ] : %s Bhukti\n", ad_start_str.c_str(), ad_end_str.c_str(), p_names_full[ad_p]);
                    printf("        %s\n", get_dynamic_bhukti(md_p, ad_p, ad_score, ad_house).c_str());
                    
                    // Inject the direct Life-Event prediction
                    if (ad_p == 8 || ad_p == 9) {
                        printf("        * Life Events: %s\n", get_node_bhukti_event(p_names_full[ad_p], ad_house).c_str());
                    } else {
                        printf("        * Life Events: %s\n", get_lordship_bhukti_event(p_names_full[ad_p], owned_houses).c_str());
                    }
                }
                ad_start += ad_dur;
            }
            printf("\n-----------------------------------------------------------------------------------------\n\n");
            cur_start += md_dur;
        }
    }

    void dfs_find_dehas(int level, int current_lord, double start_jd, double duration, double target_start, double target_end, vector<int> path) {
        if (start_jd >= target_end || start_jd + duration <= target_start) return;
        if (level == 6) {
            string prefix = "";
            for (int i = 0; i < 5; i++) { prefix += short_dasha[path[i]]; if (i < 4) prefix += "-"; }
            printf("%-20s | %-10s | %-19s | %-19s\n", prefix.c_str(), dasha_lords[path[5]], jd_to_string(start_jd).c_str(), jd_to_string(start_jd + duration).c_str());
            return;
        }
        double loop_start = start_jd;
        for (int i = 0; i < 9; i++) {
            int p_idx = (current_lord + i) % 9;
            double sub_dur = duration * (dasha_years[p_idx] / 120.0);
            vector<int> new_path = path; new_path.push_back(p_idx);
            dfs_find_dehas(level + 1, p_idx, loop_start, sub_dur, target_start, target_end, new_path);
            loop_start += sub_dur;
        }
    }

    void calculate_daily_dehas(int t_year, int t_month, int t_day, int t_hour, int t_min, int t_sec, bool use_current_date, bool time_provided) {
        double target_start_jd, target_end_jd;
        if (use_current_date) {
            time_t t = time(nullptr); tm* now_utc = gmtime(&t);
            double ut_dec = now_utc->tm_hour + (now_utc->tm_min / 60.0) + (now_utc->tm_sec / 3600.0);
            target_start_jd = swe_julday(now_utc->tm_year + 1900, now_utc->tm_mon + 1, now_utc->tm_mday, ut_dec, SE_GREG_CAL);
            target_end_jd = target_start_jd + (1.0 / 86400.0); 
            printf("\n=== PINPOINT DEHA DASHA FOR EXACT CURRENT CLOCK ===\n");
        } else {
            if (time_provided) {
                double ut_dec = t_hour + (t_min / 60.0) + (t_sec / 3600.0) - location.tz_offset;
                target_start_jd = swe_julday(t_year, t_month, t_day, ut_dec, SE_GREG_CAL);
                target_end_jd = target_start_jd + (1.0 / 86400.0); 
                printf("\n=== PINPOINT DEHA DASHA FOR %02d/%02d/%04d %02d:%02d:%02d ===\n", t_day, t_month, t_year, t_hour, t_min, t_sec);
            } else {
                target_start_jd = swe_julday(t_year, t_month, t_day, 0.0 - location.tz_offset, SE_GREG_CAL);
                target_end_jd = target_start_jd + 1.0;
                printf("\n=== DAILY DEHA DASHA SWEEP ===\n");
            }
        }
        printf("--------------------------------------------------------------------------------------\n");
        printf("%-20s | %-10s | %-19s | %-19s\n", "M-A-P-S-Pr Path", "Deha Lord", "Starts", "Ends");
        printf("--------------------------------------------------------------------------------------\n");

        double nak_size = 360.0 / 27.0; int nak_index = (int)(moon_lon / nak_size); int lord_index = nak_index % 9;
        double fraction_passed = (moon_lon - (nak_index * nak_size)) / nak_size;
        double life_start_jd = tjd_ut - (fraction_passed * dasha_years[lord_index] * 365.2425);
        vector<int> path;
        dfs_find_dehas(0, lord_index, life_start_jd, 120.0 * 365.2425, target_start_jd, target_end_jd, path);
        printf("--------------------------------------------------------------------------------------\n");
    }

    void export_all_dashas_csv() {
        double nak_size = 360.0 / 27.0; int nak_index = (int)(moon_lon / nak_size); int lord_index = nak_index % 9;
        double fraction_passed = (moon_lon - (nak_index * nak_size)) / nak_size;
        double life_start_jd = tjd_ut - (fraction_passed * dasha_years[lord_index] * 365.2425);

        FILE* fp = fopen("vimshottari_full_export.csv", "w");
        if (!fp) { printf("Error creating CSV file!\n"); return; }
        fprintf(fp, "Level_Depth,Level_Name,Path,Active_Lord,Start_Date,End_Date\n");
        printf("\n=== EXPORTING 120-YEAR DASHA HIERARCHY ===\nGenerating roughly 598,000 records... Please wait...\n");

        vector<string> path_names;
        dfs_export_csv(0, lord_index, life_start_jd, 120.0 * 365.2425, path_names, fp);
        fclose(fp);
        printf("Success! All dashas have been saved to 'vimshottari_full_export.csv'.\n");
    }

    void dfs_export_csv(int level, int current_lord, double start_jd, double duration, vector<string> path, FILE* fp) {
        if (level >= 6) return;
        double loop_start = start_jd;
        for (int i = 0; i < 9; i++) {
            int p_idx = (current_lord + i) % 9;
            double sub_dur = duration * (dasha_years[p_idx] / 120.0);
            vector<string> new_path = path; new_path.push_back(short_dasha[p_idx]);
            string path_str = "";
            for(size_t j = 0; j < new_path.size(); j++) { path_str += new_path[j]; if(j < new_path.size() - 1) path_str += "-"; }
            fprintf(fp, "%d,%s,%s,%s,%s,%s\n", level + 1, dasha_levels[level], path_str.c_str(), dasha_lords[p_idx], jd_to_string(loop_start).c_str(), jd_to_string(loop_start + sub_dur).c_str());
            dfs_export_csv(level + 1, p_idx, loop_start, sub_dur, new_path, fp);
            loop_start += sub_dur;
        }
    }
// Phase 3: Silent Dasha Fetcher
    void get_active_dasha_lords(double target_jd, int &md_lord, int &ad_lord) const {
        double nak_size = 360.0 / 27.0; 
        int nak_index = (int)(moon_lon / nak_size);
        int lord_index = nak_index % 9;
        double fraction_passed = (moon_lon - (nak_index * nak_size)) / nak_size;
        double life_start_jd = tjd_ut - (fraction_passed * dasha_years[lord_index] * 365.2425);

        double cur_start = life_start_jd; 
        double cur_dur = 120.0 * 365.2425; 
        int cur_lord = lord_index;

        // Find Mahadasha (MD)
        for (int i = 0; i < 9; i++) {
            int p_idx = (cur_lord + i) % 9;
            double sub_dur = cur_dur * (dasha_years[p_idx] / 120.0);
            if (target_jd >= cur_start && target_jd < cur_start + sub_dur) {
                md_lord = p_idx; cur_dur = sub_dur; cur_lord = p_idx; break;
            }
            cur_start += sub_dur;
        }

        // Find Antardasha (AD)
        double ad_start = cur_start;
        for (int i = 0; i < 9; i++) {
            int p_idx = (cur_lord + i) % 9;
            double sub_dur = cur_dur * (dasha_years[p_idx] / 120.0);
            if (target_jd >= ad_start && target_jd < ad_start + sub_dur) {
                ad_lord = p_idx; break;
            }
            ad_start += sub_dur;
        }
    }

	double calculate_tithi_return(int target_year) {
        printf("\n=== TITHI PRAVESHA (VEDIC BIRTHDAY ENGINE) ===\n");
        
        // 1. Calculate Natal Baseline
        double natal_angle = fmod(moon_lon - sun_lon + 360.0, 360.0);
        int natal_sun_sign = (int)(sun_lon / 30.0);
        int tithi_idx = (int)(natal_angle / 12.0);
        string paksha = (tithi_idx < 15) ? "Shukla" : "Krishna";
        
        printf("Natal Sun Sign       : %s\n", rashi_names[natal_sun_sign]);
        printf("Natal Sun-Moon Angle : %02d° %02d'\n", (int)natal_angle, (int)((natal_angle - (int)natal_angle) * 60.0));
        printf("Natal Tithi          : %s (%s)\n", tithi_names[tithi_idx], paksha.c_str());
        printf("Target Year          : %d\n", target_year);
        printf("-----------------------------------------------------------------\n");

        // 2. Find the ~30 day window where the Sun is in the Natal Sign (Cancer) for the target year
        double search_start = swe_julday(target_year, 1, 1, 0.0, SE_GREG_CAL);
        double window_start = 0, window_end = 0;
        bool in_window = false;
        
        for (double jd = search_start; jd < search_start + 365.0; jd += 1.0) {
            double xx[6]; char serr[256];
            swe_calc_ut(jd, SE_SUN, iflag, xx, serr);
            int s_sign = (int)(xx[0] / 30.0);
            
            if (s_sign == natal_sun_sign && !in_window) {
                window_start = jd - 2.0; // Pad by a couple of days
                in_window = true;
            } else if (s_sign != natal_sun_sign && in_window) {
                window_end = jd + 2.0;
                break;
            }
        }
        
        // 3. Sweep the 30-day window to find the exact Sun-Moon Angle match
        double step = 3.0 / 24.0; // 3-hour steps
        double exact_jd = 0;
        bool found = false;
        
        for (double jd = window_start; jd <= window_end; jd += step) {
            double s_xx1[6], m_xx1[6], s_xx2[6], m_xx2[6]; char serr[256];
            swe_calc_ut(jd, SE_SUN, iflag, s_xx1, serr);
            swe_calc_ut(jd, SE_MOON, iflag, m_xx1, serr);
            swe_calc_ut(jd + step, SE_SUN, iflag, s_xx2, serr);
            swe_calc_ut(jd + step, SE_MOON, iflag, m_xx2, serr);
            
            double a1 = fmod(m_xx1[0] - s_xx1[0] + 360.0, 360.0);
            double a2 = fmod(m_xx2[0] - s_xx2[0] + 360.0, 360.0);
            
            // Handle 360-degree wrap-around (e.g., angle goes from 359 to 1)
            if (a2 < a1 && (a1 > 340.0 && a2 < 20.0)) a2 += 360.0;
            double target_a = natal_angle;
            if (target_a < a1 && (a1 > 340.0 && target_a < 20.0)) target_a += 360.0;
            
            if (a1 <= target_a && a2 >= target_a) {
                // Bisection: Drill down to the exact second
                double t_low = jd, t_high = jd + step;
                for (int i = 0; i < 40; i++) {
                    double t_mid = (t_low + t_high) / 2.0;
                    double s_mid[6], m_mid[6];
                    swe_calc_ut(t_mid, SE_SUN, iflag, s_mid, serr);
                    swe_calc_ut(t_mid, SE_MOON, iflag, m_mid, serr);
                    double a_mid = fmod(m_mid[0] - s_mid[0] + 360.0, 360.0);
                    
                    if (a_mid < a1 && (a1 > 340.0 && a_mid < 20.0)) a_mid += 360.0;
                    
                    if (a_mid < target_a) t_low = t_mid;
                    else t_high = t_mid;
                }
                exact_jd = (t_low + t_high) / 2.0;
                found = true;
                break;
            }
        }
        
		// Replace the bottom of calculate_tithi_return with this:
        if (found) {
            printf("Exact Tithi Return Time : %s (Local Time)\n", jd_to_string(exact_jd).c_str());
            printf("Status                  : HIGH IMPORTANCE - Annual Cycle Reset Point\n");
            
            double s_pos[6], m_pos[6]; char serr[256];
            swe_calc_ut(exact_jd, SE_SUN, iflag, s_pos, serr);
            swe_calc_ut(exact_jd, SE_MOON, iflag, m_pos, serr);
            printf("\n[Planetary Posture at Reset]\n");
            printf("Transit Surya   : %s\n", format_dms(s_pos[0]).c_str());
            printf("Transit Chandra : %s\n", format_dms(m_pos[0]).c_str());
            printf("-----------------------------------------------------------------\n");
            
            return exact_jd; 
        } else {
            printf("Error: Could not calculate Tithi Return for this year.\n");
            printf("-----------------------------------------------------------------\n");
            return 0.0;
        }
    } // End of function
	
void calculate_transits(int t_year, int t_month, int t_day, int t_hour, int t_min, int t_sec, bool use_current_date, bool is_web_mode = false) {
        double trans_jd; int p_y = t_year, p_m = t_month, p_d = t_day, p_h = t_hour, p_min = t_min, p_s = t_sec;
        if (use_current_date) {
            time_t t = time(nullptr); tm* now_utc = gmtime(&t);
            double ut_dec = now_utc->tm_hour + (now_utc->tm_min / 60.0) + (now_utc->tm_sec / 3600.0);
            trans_jd = swe_julday(now_utc->tm_year + 1900, now_utc->tm_mon + 1, now_utc->tm_mday, ut_dec, SE_GREG_CAL);
            int y, m, d; double jut; swe_revjul(trans_jd + (location.tz_offset / 24.0), SE_GREG_CAL, &y, &m, &d, &jut);
            p_y = y; p_m = m; p_d = d; p_h = (int)jut; p_min = (int)((jut - p_h) * 60.0); p_s = (int)round((((jut - p_h) * 60.0) - p_min) * 60.0);
        } else {
            double ut_dec = t_hour + (t_min / 60.0) + (t_sec / 3600.0) - location.tz_offset;
            trans_jd = swe_julday(t_year, t_month, t_day, ut_dec, SE_GREG_CAL);
        }

        if (!av_calculated) calculate_ashtakavarga(true); 
        
        int md_lord = -1, ad_lord = -1;
        get_active_dasha_lords(trans_jd, md_lord, ad_lord);

        if (telugu_mode) {
            printf("\n=== గోచారం (గ్రహ సంచారం) తేదీ: %02d/%02d/%04d సమయం: %02d:%02d:%02d ===\n", p_d, p_m, p_y, p_h, p_min, p_s);
            printf("ప్రస్తుత దశ: %s మహా దశ / %s అంతర్ దశ\n", get_dasha_lord(md_lord).c_str(), get_dasha_lord(ad_lord).c_str());
            printf("------------------------------------------------------------------------------------------------------------------------------------------\n");
            printf("%-10s | %-15s | %-20s | %-12s | %-12s | %-12s | %-25s\n", "గ్రహం", "సంచార రాశి", "తార (నవతార)", "జన్మ రాశి", "చంద్రుని నుండి", "లగ్నం నుండి", "చూసే రాశులు (దృష్టి)");
        } else {
            printf("\n=== PLANETARY TRANSITS (GOCHAR) FOR %02d/%02d/%04d %02d:%02d:%02d ===\n", p_d, p_m, p_y, p_h, p_min, p_s);
            printf("Current Operating Dasha: %s Mahadasha / %s Antardasha\n", dasha_lords[md_lord], dasha_lords[ad_lord]);
            printf("------------------------------------------------------------------------------------------------------------------------------------------\n");
            printf("%-8s | %-15s | %-20s | %-12s | %-12s | %-12s | %-25s\n", "Graha", "Transit Sign", "Tara (Navatara)", "Natal Sign", "From Natal Mo", "From Natal Asc", "Aspected Signs");
        }
        printf("------------------------------------------------------------------------------------------------------------------------------------------\n");

        double t_cusps[13], t_ascmc[10];
        swe_houses_ex(trans_jd, iflag, location.lat, location.lon, 'P', t_cusps, t_ascmc);
        double t_lagna = t_ascmc[0];
        string t_lagna_sign = format_dms(t_lagna);
        printf("%-10s | %-15s | %-20s | %-12s | %-12s | %-12s | %-25s\n", telugu_mode ? "లగ్నం" : "Lagna", t_lagna_sign.c_str(), "-", "-", "-", "-", "-");

        int planets[] = {SE_SUN, SE_MOON, SE_MARS, SE_MERCURY, SE_JUPITER, SE_VENUS, SE_SATURN, SE_TRUE_NODE};
        double xx[6]; char serr[256];
        int natal_mo_rashi = planet_rashis[2]; int natal_asc_rashi = planet_rashis[0]; 
        int natal_mo_nak = (int)(moon_lon / (360.0 / 27.0));

        map<int, vector<TransitHit>> transit_triggers; 
        int t_rashis[10];

        for (int i = 1; i <= 9; i++) {
            double trans_lon;
            if (i < 9) { swe_calc_ut(trans_jd, planets[i-1], iflag, xx, serr); trans_lon = xx[0]; } 
            else { swe_calc_ut(trans_jd, SE_TRUE_NODE, iflag, xx, serr); trans_lon = fmod(xx[0] + 180.0, 360.0); }
            
            int trans_rashi = (int)(trans_lon / 30.0); t_rashis[i] = trans_rashi;
            double trans_deg = trans_lon - (trans_rashi * 30.0);
            int deg = (int)trans_deg; int min = (int)((trans_deg - deg) * 60.0);
            int trans_nak = (int)(trans_lon / (360.0 / 27.0));
            int tara_idx = (trans_nak - natal_mo_nak + 27) % 9;

            int nat_rashi = planet_rashis[i];
            int from_mo = (trans_rashi - natal_mo_rashi + 12) % 12 + 1;
            int from_asc = (trans_rashi - natal_asc_rashi + 12) % 12 + 1;

            string t_sign = format_dms(trans_lon);
            string t_name = get_tara(tara_idx);
            string short_tara = t_name.substr(0, t_name.find(" ("));

            vector<int> a_rashis; int r = trans_rashi;
            a_rashis.push_back((r + 6) % 12); 
            if (i == 3) { a_rashis.push_back((r + 3) % 12); a_rashis.push_back((r + 7) % 12); } 
            else if (i == 5 || i == 8 || i == 9) { a_rashis.push_back((r + 4) % 12); a_rashis.push_back((r + 8) % 12); } 
            else if (i == 7) { a_rashis.push_back((r + 2) % 12); a_rashis.push_back((r + 9) % 12); } 

            string asp_str = "";
            for (size_t a = 0; a < a_rashis.size(); a++) {
                asp_str += get_rashi_name(a_rashis[a]);
                if (a < a_rashis.size() - 1) asp_str += ", ";
            }

            printf("%-10s | %-15s | %-20s | %-12s | %s %-3d | %s %-3d | %-25s\n", 
                get_planet_name(i).c_str(), t_sign.c_str(), short_tara.c_str(), get_rashi_name(nat_rashi).c_str(), telugu_mode ? "భావం" : "House", from_mo, telugu_mode ? "భావం" : "House", from_asc, asp_str.c_str());
       
			for (int np = 0; np <= 9; np++) {
                if (planet_rashis[np] == trans_rashi) {
                    transit_triggers[i].push_back({np == 0 ? (telugu_mode ? "లగ్నం" : "Lagna") : get_planet_name(np), telugu_mode ? "కలయిక (1వ భావం)" : "Conjuncts (1st House hit)"});
                }
            }
            
            for (int asp_rashi : a_rashis) {
                for (int np = 0; np <= 9; np++) {
                    if (planet_rashis[np] == asp_rashi) {
                        int asp_num = (asp_rashi - r + 12) % 12 + 1;
                        transit_triggers[i].push_back({np == 0 ? (telugu_mode ? "లగ్నం" : "Lagna") : get_planet_name(np), telugu_mode ? "దృష్టి (" + to_string(asp_num) + "వ భావం)" : "Aspects (" + to_string(asp_num) + "th House hit)"});
                    }
                }
            }
            
            // ==========================================
            // NEW: GOCHARA TIMELINE SWEEPER & TEXT INJECTION
            // ==========================================
            if (i == 1 || i == 3 || i == 5 || i == 7 || i == 8 || i == 9) {
                double step = (i == 1 || i == 3) ? 1.0 : 5.0; // Fast planets step by 1 day, Slow planets by 5 days
                
                // Sweep backwards to find Entry Date
                double jd_in = trans_jd;
                while ((int)(get_planet_lon_on_jd(i, jd_in) / 30.0) == trans_rashi) jd_in -= step;
                while ((int)(get_planet_lon_on_jd(i, jd_in) / 30.0) != trans_rashi) jd_in += 1.0;

                // Sweep forwards to find Exit Date
                double jd_out = trans_jd;
                while ((int)(get_planet_lon_on_jd(i, jd_out) / 30.0) == trans_rashi) jd_out += step;
                while ((int)(get_planet_lon_on_jd(i, jd_out) / 30.0) != trans_rashi) jd_out -= 1.0;
                
                string date_range = "[ " + jd_to_string(jd_in).substr(0,10) + "  >>  " + jd_to_string(jd_out).substr(0,10) + " ]";
                
                string gochar_payload = (telugu_mode ? "కాల వ్యవధి: " : "Timeline: ") + date_range + "\n        " + (telugu_mode ? te_get_gochar_text(i, from_mo) : get_gochar_text(i, from_mo));
                
                transit_triggers[i].push_back({"GOCHAR_RESULT", gochar_payload});
            }
        }
        printf("------------------------------------------------------------------------------------------------------------------------------------------\n");

		printf("\n[PHASE 3: TRANSIT SYNTHESIS (Dasha, AV Filtering & Gochara Phala)]\n");

        if (telugu_mode) {
            printf("=========================================================================================\n");
            printf("=== గోచార ఫలితాలు (DYNAMIC TRANSIT PREDICTIONS) ===\n");
            printf("=========================================================================================\n");
        } else {
            printf("=========================================================================================\n");
            printf("=== DYNAMIC TRANSIT PREDICTIONS (GOCHARA PHALA) ===\n");
            printf("=========================================================================================\n");
        }

        int major_planets[] = {1, 3, 5, 7, 8, 9}; // Sun, Mars, Jup, Sat, Rahu, Ketu
        
        // =========================================================================
        // PART A: THE NARRATIVE TIMELINE (For the User)
        // =========================================================================
        for (int i = 0; i < 6; i++) {
            int mp = major_planets[i];
            if (transit_triggers.find(mp) != transit_triggers.end() && !transit_triggers[mp].empty()) {
                
                string gochar_text = "";
                for (auto hit : transit_triggers[mp]) {
                    if (hit.p_name == "GOCHAR_RESULT") {
                        gochar_text = hit.hit_type;
                        break; 
                    }
                }
                
                if (gochar_text != "") {
                    int d_map[] = {-1, 2, 3, 4, 8, 6, 1, 7, 5, 0}; 
                    bool is_dasha_lord = (d_map[mp] == md_lord || d_map[mp] == ad_lord);
                    string dasha_alert = is_dasha_lord ? (telugu_mode ? " [ప్రస్తుత దశా నాథుడు]" : " [ACTIVE DASHA LORD]") : "";

                    string av_alert = "";
                    if (mp < 8) {
                        int r = t_rashis[mp];
                        int r_sav = sav_scores[r];
                        if (r_sav >= 28) av_alert = telugu_mode ? " | శుభ బలం (SAV: " + to_string(r_sav) + ")" : " | High Support (SAV: " + to_string(r_sav) + ")";
                        else if (r_sav <= 24) av_alert = telugu_mode ? " | ప్రతికూలం (SAV: " + to_string(r_sav) + ")" : " | Low Support (SAV: " + to_string(r_sav) + ")";
                        else av_alert = telugu_mode ? " | మధ్యస్థం (SAV: " + to_string(r_sav) + ")" : " | Average (SAV: " + to_string(r_sav) + ")";
                    }

                    if (telugu_mode) {
                        printf("\n⭐ %s గమనము (ప్రస్తుతం %dవ భావంలో)%s%s\n", get_planet_name(mp).c_str(), (t_rashis[mp] - natal_mo_rashi + 12) % 12 + 1, dasha_alert.c_str(), av_alert.c_str());
                        printf("   %s\n", gochar_text.c_str());
                    } else {
                        printf("\n⭐ %s TRANSIT (Currently in %dth House)%s%s\n", p_names_full[mp], (t_rashis[mp] - natal_mo_rashi + 12) % 12 + 1, dasha_alert.c_str(), av_alert.c_str());
                        printf("   %s\n", gochar_text.c_str());
                    }
                }
            }
        }
        
        // =========================================================================
        // PART B: TECHNICAL TELEMETRY (For the Developer/Astrologer to trace)
        // =========================================================================
        if (telugu_mode) {
            printf("\n------------------------------------------------------------------------------------------------------------------------------------------\n");
            printf("[సాంకేతిక గోచార దృష్టి (TECHNICAL TRANSIT HITS ON NATAL CHART)]\n");
        } else {
            printf("\n------------------------------------------------------------------------------------------------------------------------------------------\n");
            printf("[TECHNICAL TRANSIT HITS ON NATAL CHART]\n");
        }

        for (int i = 0; i < 6; i++) {
            int mp = major_planets[i];
            if (transit_triggers.find(mp) != transit_triggers.end() && !transit_triggers[mp].empty()) {
                
                // Check if there are any actual technical hits (ignoring the narrative payload)
                bool has_hits = false;
                for (auto hit : transit_triggers[mp]) {
                    if (hit.p_name != "GOCHAR_RESULT") { has_hits = true; break; }
                }

                if (has_hits) {
                    int d_map[] = {-1, 2, 3, 4, 8, 6, 1, 7, 5, 0}; 
                    bool is_dasha_lord = (d_map[mp] == md_lord || d_map[mp] == ad_lord);
                    string dasha_alert = is_dasha_lord ? (telugu_mode ? " [ప్రస్తుత దశా నాథుడు]" : " [ACTIVE DASHA LORD]") : "";

                    string av_alert = "";
                    if (mp < 8) {
                        int r = t_rashis[mp];
                        int r_sav = sav_scores[r];
                        int r_bav = bav_scores[mp-1][r]; // Bringing back the specific BAV for the technical print
                        if (r_sav >= 28 && r_bav >= 4) av_alert = telugu_mode ? " -> శుభ బలం (SAV: " + to_string(r_sav) + ", BAV: " + to_string(r_bav) + ")" : " -> High Support (SAV: " + to_string(r_sav) + ", BAV: " + to_string(r_bav) + ")";
                        else if (r_sav <= 24 || r_bav <= 2) av_alert = telugu_mode ? " -> తీవ్ర ప్రతికూలం (Weak AV! SAV: " + to_string(r_sav) + ", BAV: " + to_string(r_bav) + ")" : " -> HIGH FRICTION (Weak AV! SAV: " + to_string(r_sav) + ", BAV: " + to_string(r_bav) + ")";
                        else av_alert = telugu_mode ? " -> మధ్యస్థం (SAV: " + to_string(r_sav) + ")" : " -> Average Environment (SAV: " + to_string(r_sav) + ")";
                    }

                    if (telugu_mode) printf("  => గోచార %s%s%s కింది గ్రహాలను (జన్మ కుండలిలో) తాకుతోంది:\n", get_planet_name(mp).c_str(), dasha_alert.c_str(), av_alert.c_str());
                    else printf("  => Transit %s%s%s is hitting:\n", p_names_full[mp], dasha_alert.c_str(), av_alert.c_str());
                    
                    for (auto hit : transit_triggers[mp]) {
                        if (hit.p_name != "GOCHAR_RESULT") {
                            if (telugu_mode) printf("     * %-25s -> జన్మ %s\n", hit.hit_type.c_str(), hit.p_name.c_str());
                            else printf("     * %-30s -> Natal %s\n", hit.hit_type.c_str(), hit.p_name.c_str());
                        }
                    }
                }
            }
        }
        printf("------------------------------------------------------------------------------------------------------------------------------------------\n");
        // ==============================================================================
        // [PHASE 4: VULNERABILITY & DESTRUCTION MATRIX]
        // ==============================================================================
        
		if (!is_web_mode) {
				
			printf("\n[PHASE 4: VULNERABILITY & DESTRUCTION MATRIX]\n");

			int mb_degrees[10][12] = {
				{ 1,  9, 22, 22, 25,  2,  4, 23, 18, 20, 24, 10}, 
				{20,  9, 12,  6,  8, 24, 16, 17, 22,  2,  3, 23}, 
				{26, 12, 13, 25, 24, 11, 26, 14, 13, 25,  5, 12}, 
				{19, 28, 25, 23, 29, 28, 14, 21,  2, 15, 11,  6}, 
				{15, 14, 13, 12,  8, 18, 20, 10, 21, 22,  7,  5}, 
				{19, 29, 12, 27,  6,  4, 13, 10, 17, 11, 15, 28}, 
				{28, 15, 11, 17, 10, 13,  4,  6, 27, 12, 29, 19}, 
				{10,  4,  7,  9, 12, 16,  3, 18, 28, 14, 13, 15}, 
				{14, 13, 12, 11, 24, 23, 22, 21, 10, 20, 18,  8}, 
				{ 8, 18, 20, 10, 21, 22, 23, 24, 11, 12, 13, 14}  
			};

			double nav_64_lon = fmod(moon_lon + 210.0, 360.0);
			double drek_22_lon = fmod(lagna_lon + 210.0, 360.0);
			double bhrigu_bindu = fmod((moon_lon + planet_lons[8]) / 2.0, 360.0); 

			int d9_lagna_rashi = (int)(fmod(lagna_lon * 9.0, 360.0) / 30.0);
			double d9_proj_lon = (d9_lagna_rashi * 30.0) + fmod(lagna_lon, 30.0);

			double geopos[3] = {location.lon, location.lat, 0.0};
			double trise, tset, next_rise; char serr_g[256];
			swe_rise_trans(tjd_ut - 0.5, SE_SUN, NULL, iflag, SE_CALC_RISE, geopos, 0, 0, &trise, serr_g);
			swe_rise_trans(tjd_ut - 0.5, SE_SUN, NULL, iflag, SE_CALC_SET, geopos, 0, 0, &tset, serr_g);
			swe_rise_trans(tjd_ut + 0.5, SE_SUN, NULL, iflag, SE_CALC_RISE, geopos, 0, 0, &next_rise, serr_g);

			int weekday = (int)(floor(tjd_ut + location.tz_offset / 24.0 + 1.5)) % 7; 
			bool is_day_birth = (tjd_ut >= trise && tjd_ut < tset);
			double duration = is_day_birth ? (tset - trise) : (next_rise - tset);
			int saturn_part = (6 - (is_day_birth ? weekday : (weekday + 4) % 7) + 7) % 7;
			double gulika_ut = (is_day_birth ? trise : tset) + (saturn_part * (duration / 8.0));
			
			double g_cusps[13], g_ascmc[10];
			swe_houses_ex(gulika_ut, iflag, location.lat, location.lon, 'P', g_cusps, g_ascmc);
			double gulika_lon = g_ascmc[0];

			int l_rashi = planet_rashis[0];
			int maraka_2 = (l_rashi + 1) % 12;
			int maraka_7 = (l_rashi + 6) % 12;

			int badhaka_rashi;
			if (l_rashi % 3 == 0) badhaka_rashi = (l_rashi + 10) % 12;     
			else if (l_rashi % 3 == 1) badhaka_rashi = (l_rashi + 8) % 12; 
			else badhaka_rashi = (l_rashi + 6) % 12;                       

			// --- CALC AVAYOGI POINT ---
			double yogi_point_calc = fmod((sun_lon + moon_lon + 93.3333333), 360.0);
			double avayogi_point = fmod((yogi_point_calc + 186.6666667), 360.0);
			double avayogi_ni_point = fmod((yogi_point_calc + 80.0), 360.0); // North Indian Concept

			struct DangerPoint { string name; double lon; };
			vector<DangerPoint> danger_points = {
				{telugu_mode ? "64వ నవాంశ (కర్మ ప్రమాద బిందువు)" : "64th Navamsha (Karmic Danger Point)", nav_64_lon},
				{telugu_mode ? "22వ ద్రేక్కాణ (సంక్షోభ బిందువు)" : "22nd Drekkana (Crisis & Fatigue Point)", drek_22_lon},
				{telugu_mode ? "D9 లగ్న ప్రొజెక్షన్" : "D9 Lagna Physical Projection", d9_proj_lon},
				{telugu_mode ? "భృగు బిందు (విధి మలుపు)" : "Bhrigu Bindu (Destiny Midpoint)", bhrigu_bindu},
				{telugu_mode ? "గుళిక (విష బిందువు)" : "Gulika (Fatal Poison Node)", gulika_lon},
				{telugu_mode ? "బాధక కచ్చితమైన డిగ్రీ" : "Badhaka Exact Degree", (badhaka_rashi * 30.0) + fmod(lagna_lon, 30.0)},
				{telugu_mode ? "అవయోగి బిందువు (ఆర్థిక/శక్తి క్షీణత)" : "Avayogi Point (Wealth/Energy Drain)", avayogi_point},
				{telugu_mode ? "ఉత్తర భారత అవయోగి (నార్త్ ఇండియన్)" : "Avayogi Point (North Indian Variant)", avayogi_ni_point},
				{telugu_mode ? "8వ భావ ప్రారంభ బిందువు (రంధ్ర స్థానం)" : "8th House Cusp (Randhra / Fatality)", house_cusps[8]} // NEW: 8th Cusp
			};

			// --- NEW: 2. Upagrahas (Maandi & Yamaghantaka via Gulika offsets) ---
			double maandi_lon = fmod(gulika_lon - 15.0 + 360.0, 360.0);
			double yama_lon = fmod(gulika_lon + 45.0, 360.0);
			danger_points.push_back({telugu_mode ? "మాంది (ప్రాణ సంక్షోభం)" : "Maandi (Sudden Shock/Surgery)", maandi_lon});
			danger_points.push_back({telugu_mode ? "యమఘంటక (ప్రమాద బిందువు)" : "Yamaghantaka (Critical Care Trigger)", yama_lon});

			// --- NEW: 5. Sahams (Arabic Parts for Death & Disease) ---
			double mrityu_saham = fmod(lagna_lon + house_cusps[8] - moon_lon + 360.0, 360.0);
			double roga_saham = fmod(lagna_lon + house_cusps[6] - moon_lon + 360.0, 360.0);
			if (!is_day_birth) {
				mrityu_saham = fmod(lagna_lon + moon_lon - house_cusps[8] + 360.0, 360.0);
				roga_saham = fmod(lagna_lon + moon_lon - house_cusps[6] + 360.0, 360.0);
			}
			danger_points.push_back({telugu_mode ? "మృత్యు సహం (ప్రాణ గండం)" : "Mrityu Saham (Fatality Trigger)", mrityu_saham});
			danger_points.push_back({telugu_mode ? "రోగ సహం (వ్యాధి బిందువు)" : "Roga Saham (Disease Trigger)", roga_saham});

			// --- DYNAMIC LORDS INJECTION (Kharesha, Trik Lords, Marakas) ---
			int drek_22_rashi = ((int)(drek_22_lon / 30.0)) % 12;
			int nav_64_rashi = ((int)(nav_64_lon / 30.0)) % 12;
			
			for (int p = 1; p <= 7; p++) {
				string p_lord_name = p_names_full[p];
				double p_lon = planet_lons[p];
				
				if (p_lord_name == rashi_lords[maraka_2]) danger_points.push_back({telugu_mode ? "2వ భావాధిపతి (ప్రాథమిక మారక)" : "2nd Lord (Primary Maraka)", p_lon});
				if (p_lord_name == rashi_lords[maraka_7]) danger_points.push_back({telugu_mode ? "7వ భావాధిపతి (ద్వితీయ మారక)" : "7th Lord (Secondary Maraka)", p_lon});
				if (p_lord_name == rashi_lords[badhaka_rashi]) danger_points.push_back({telugu_mode ? "బాధకాధిపతి (అడ్డంకి)" : "Badhaka Lord (Obstruction)", p_lon});
				
				// NEW: 1. Kharesha & 64th Lord
				if (p_lord_name == rashi_lords[drek_22_rashi]) danger_points.push_back({telugu_mode ? "ఖరేశ (22వ ద్రేక్కాణాధిపతి)" : "Kharesha (Lord of 22nd Drekkana)", p_lon});
				if (p_lord_name == rashi_lords[nav_64_rashi]) danger_points.push_back({telugu_mode ? "64వ నవాంశాధిపతి" : "64th Navamsha Lord", p_lon});
				
				// NEW: 3. Trik Lords (6th and 12th)
				if (p_lord_name == rashi_lords[(l_rashi + 5)%12]) danger_points.push_back({telugu_mode ? "6వ భావాధిపతి (రోగ స్థానం)" : "6th Lord (Roga/Disease)", p_lon});
				if (p_lord_name == rashi_lords[(l_rashi + 11)%12]) danger_points.push_back({telugu_mode ? "12వ భావాధిపతి (వ్యయ స్థానం)" : "12th Lord (Loss/Hospital)", p_lon});
			}

			// --- MRITYU BHAGAS & NEW SPECIFIC NATAL PLANET AFFLICTIONS ---
			for (int p = 0; p <= 9; p++) {
				double lon = planet_lons[p];
				int rashi = planet_rashis[p];
				double deg = fmod(lon, 30.0);
				string p_name = (p == 0) ? (telugu_mode ? "లగ్న" : "Lagna") : get_planet_name(p);
				
				// Standard BPHS Mrityu Bhaga
				double mb_absolute_lon = (rashi * 30.0) + mb_degrees[p][rashi];
				danger_points.push_back({p_name + (telugu_mode ? " మృత్యు భాగ" : "'s Mrityu Bhaga"), mb_absolute_lon});

				// NEW: 4. Gandanta (Extreme Karmic Knots)
				bool is_gandanta = false;
				if ((rashi == 3 || rashi == 7 || rashi == 11) && deg >= 29.0) is_gandanta = true; // Last degree of Water
				if ((rashi == 0 || rashi == 4 || rashi == 8) && deg <= 1.0) is_gandanta = true; // First degree of Fire
				if (is_gandanta) danger_points.push_back({(telugu_mode ? "గండాంత గ్రహం: " : "Gandanta Planet: ") + p_name, lon});

				// NEW: 4. Sarpa Drekkana
				int drek = (int)(deg / 10.0) + 1;
				if ((rashi == 3 && (drek == 1 || drek == 2)) || (rashi == 7 && (drek == 1 || drek == 2)) || (rashi == 11 && drek == 3)) {
					danger_points.push_back({(telugu_mode ? "సర్ప ద్రేక్కాణ గ్రహం: " : "Sarpa Drekkana Planet: ") + p_name, lon});
				}

				// NEW: 8. Nakshatra Vedha (Vipat, Pratyak, Vadha Taras)
				if (p > 0) { // Skip Lagna for Tara check
					int p_nak = (int)(lon / (360.0 / 27.0));
					int tara = (p_nak - natal_mo_nak + 27) % 9;
					if (tara == 2 || tara == 4 || tara == 6) { 
						string t_name = (tara == 2) ? "Vipat" : (tara == 4) ? "Pratyak" : "Vadha";
						string te_name = (tara == 2) ? "విపత్" : (tara == 4) ? "ప్రత్యక్" : "వధ";
						danger_points.push_back({(telugu_mode ? "ప్రతికూల తార ("+te_name+"): " : "Vedha Tara ("+t_name+"): ") + p_name, lon});
					}
				}

				// NEW: 6. Visha Navamsha Degrees
				bool is_visha = false;
				if (rashi % 3 == 0 && std::abs(deg - 10.0) <= 1.0) is_visha = true; // Movable Signs
				if (rashi % 3 == 1 && std::abs(deg - 14.0) <= 1.0) is_visha = true; // Fixed Signs
				if (rashi % 3 == 2 && std::abs(deg - 18.0) <= 1.0) is_visha = true; // Dual Signs
				if (is_visha) danger_points.push_back({(telugu_mode ? "విష నవాంశ బిందువు: " : "Visha Navamsha Point: ") + p_name, lon});
			}

			// --- NEW: 7. Graha Yuddha (Planetary War Axis) ---
			for (int p1 = 3; p1 <= 7; p1++) { // Check Mars, Merc, Jup, Ven, Sat
				for (int p2 = p1 + 1; p2 <= 7; p2++) {
					double dist = std::abs(planet_lons[p1] - planet_lons[p2]);
					if (dist > 180.0) dist = 360.0 - dist;
					if (dist <= 1.0) {
						double midpoint = fmod((planet_lons[p1] + planet_lons[p2]) / 2.0, 360.0);
						string yuddha_name = get_planet_name(p1) + " vs " + get_planet_name(p2);
						danger_points.push_back({(telugu_mode ? "గ్రహ యుద్ధం (వార్ జోన్): " : "Graha Yuddha (War Zone): ") + yuddha_name, midpoint});
					}
				}
			}
			if (telugu_mode) printf("--- జన్మ కుండలి ప్రమాద బిందువులు (క్రాస్-చెకింగ్ కోసం) ---\n");
			else printf("--- NATAL VULNERABILITY TARGETS (For Cross-Checking) ---\n");
			
			for (const auto& dp : danger_points) {
				int rashi = ((int)(fmod(dp.lon, 360.0) / 30.0)) % 12;
				double rem = fmod(dp.lon, 30.0);
				int d = (int)rem;
				int m = (int)((rem - d) * 60.0);
				int s = (int)round((((rem - d) * 60.0) - m) * 60.0);
				if (s >= 60) { s -= 60; m += 1; }
				if (m >= 60) { m -= 60; d += 1; }
				if (d >= 30) { d -= 30; rashi = (rashi + 1) % 12; }
				
				// Now prints Degrees, Minutes, and Seconds!
				printf(" * %-45s : %02d° %-7s %02d'%02d\"\n", dp.name.c_str(), d, get_rashi_name(rashi).c_str(), m, s);
			}
			printf("------------------------------------------------------------------------------------------------------------------------------------------\n");
			
			int t_planets[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
			bool destruction_triggered = false;
			double master_orb = 2.0; // WIDENED ORB FOR TIMELINE CAPTURE

			for (int m_idx : t_planets) {
				double t_lon;
				if (m_idx < 9) { swe_calc_ut(trans_jd, planets[m_idx-1], iflag, xx, serr); t_lon = xx[0]; } 
				else { swe_calc_ut(trans_jd, SE_TRUE_NODE, iflag, xx, serr); t_lon = fmod(xx[0] + 180.0, 360.0); }

				int t_rashi = (int)(t_lon / 30.0);
				double t_deg_in_rashi = t_lon - (t_rashi * 30.0);
				bool is_toxic = (std::abs(t_deg_in_rashi - mb_degrees[m_idx][t_rashi]) <= 1.0);
				string toxic_flag = is_toxic ? " [TOXIC: IN MRITYU BHAGA]" : "";

				for (const auto& dp : danger_points) {
					double dist = std::abs(t_lon - dp.lon);
					if (dist > 180.0) dist = 360.0 - dist;

					if (dist <= master_orb) { 
						destruction_triggered = true;
						string severity;
						if (m_idx == 3 || m_idx == 7 || m_idx == 8 || m_idx == 9) severity = "[!!! CRITICAL DESTROYER !!!]";
						else if (m_idx == 5) severity = "[MAJOR KARMIC TRIGGER]";
						else severity = "[EXACT DAY/TIME TRIGGER]";
						
						string warning_type = "GENERAL TRIGGER";
						if (m_idx == 3) warning_type = "SUDDEN FIRE/TRAUMA";
						else if (m_idx == 7) warning_type = "CRUSHING DELAY/LOSS";
						else if (m_idx == 8 || m_idx == 9) warning_type = "ILLUSION/EXPLOSION";
						else if (m_idx == 2 || m_idx == 4) warning_type = "DAILY EVENT TRIGGER"; 

						double e_in, e_peak, e_out;
						refine_bubble(m_idx, dp.lon, trans_jd, master_orb, e_in, e_peak, e_out);

						if (telugu_mode) {
							printf("  %s [%s] గోచార %s%s మీ %s ను తాకుతోంది\n", 
								   severity.c_str(), warning_type.c_str(), get_planet_name(m_idx).c_str(), toxic_flag.c_str(), dp.name.c_str());
							printf("      -> [కాలక్రమం] ప్రవేశం: %s | గరిష్ఠం: %s | నిష్క్రమణ: %s (వ్యత్యాసం: %.2f°)\n", 
								   jd_to_string(e_in).c_str(), jd_to_string(e_peak).c_str(), jd_to_string(e_out).c_str(), dist);
						} else {
							printf("  %s [%s] Transit %s%s is hitting your %s\n", 
								   severity.c_str(), warning_type.c_str(), p_names_full[m_idx], toxic_flag.c_str(), dp.name.c_str());
							printf("      -> [TIMELINE] Enter: %s | PEAK: %s | Exit: %s (Orb: %.2f°)\n", 
								   jd_to_string(e_in).c_str(), jd_to_string(e_peak).c_str(), jd_to_string(e_out).c_str(), dist);
						}                }
				}
			}

			if (!destruction_triggered) printf("  [System Clear] No active Maraka, Khara, Gulika, or Trika degree collisions detected for this date.\n");
			printf("------------------------------------------------------------------------------------------------------------------------------------------\n");
			
			// ==============================================================================
			// [PHASE 5: AUSPICIOUS & OPPORTUNITY MATRIX (Blessings, Yogi & Pushkara)]
			// ==============================================================================
			printf("\n[PHASE 5: AUSPICIOUS & OPPORTUNITY MATRIX]\n");

			int pushkara_degrees[12] = {21, 14, 24, 7, 21, 14, 24, 14, 24, 14, 24, 9};

			int h1_rashi_ausp = planet_rashis[0];
			int h5_rashi_ausp = (h1_rashi_ausp + 4) % 12;
			int h9_rashi_ausp = (h1_rashi_ausp + 8) % 12;

			int l1_idx_ausp = 1, l5_idx_ausp = 1, l9_idx_ausp = 1;
			for (int p=1; p<=7; p++) {
				if (string(rashi_lords[h1_rashi_ausp]) == p_names_full[p]) l1_idx_ausp = p;
				if (string(rashi_lords[h5_rashi_ausp]) == p_names_full[p]) l5_idx_ausp = p;
				if (string(rashi_lords[h9_rashi_ausp]) == p_names_full[p]) l9_idx_ausp = p;
			}

			double yogi_point_ausp = fmod((sun_lon + moon_lon + 93.3333333), 360.0);
			int y_nak_idx_ausp = (int)(yogi_point_ausp / (360.0 / 27.0));
			int lord_map_yogi_ausp[] = {9, 6, 1, 2, 3, 8, 5, 7, 4}; 
			int yogi_planet_idx_ausp = lord_map_yogi_ausp[y_nak_idx_ausp % 9];

			// --- CALC UL (UPAPADA LAGNA) FOR MARRIAGE EVENTS ---
			int h12_rashi = (planet_rashis[0] + 11) % 12;
			int l12_idx = 1; 
			for(int x = 1; x <= 7; x++) { if(string(rashi_lords[h12_rashi]) == p_names_full[x]) l12_idx = x; }
			int distance = (planet_rashis[l12_idx] - h12_rashi + 12) % 12;
			int ul_rashi = (planet_rashis[l12_idx] + distance) % 12;
			if (ul_rashi == h12_rashi || ul_rashi == (h12_rashi + 6) % 12) ul_rashi = (ul_rashi + 9) % 12; 
			double ul_lon = (ul_rashi * 30.0) + fmod(planet_lons[0], 30.0); // Exact Lagna degree projected to UL

			struct BlessingPoint { string name; double lon; };
			vector<BlessingPoint> blessing_points = {
				{telugu_mode ? "లగ్నాధిపతి (ఆరోగ్యం & వ్యక్తిత్వం)" : "Lagna Lord (Self & Vitality)", planet_lons[l1_idx_ausp]},
				{telugu_mode ? "5వ భావాధిపతి (పూర్వ పుణ్యం & అదృష్టం)" : "5th Lord (Poorva Punya & Merit)", planet_lons[l5_idx_ausp]},
				{telugu_mode ? "9వ భావాధిపతి (భాగ్యం & అదృష్టం)" : "9th Lord (Bhagya & Fortune)", planet_lons[l9_idx_ausp]},
				{telugu_mode ? "యోగి బిందువు (ఆర్థిక వృద్ధి కేంద్రం)" : "Yogi Point (Core Prosperity Axis)", yogi_point_ausp},
				{telugu_mode ? "భృగు బిందు (విధి మలుపు)" : "Bhrigu Bindu (Destiny Catalyst)", bhrigu_bindu},
				{telugu_mode ? "దారకారక (జీవిత భాగస్వామి)" : "Darakaraka (Spouse / Partnership)", planet_lons[darakaraka_idx]},
				{telugu_mode ? "ఉపపద లగ్నం (వివాహ స్థానం)" : "Upapada Lagna (Marriage Axis)", ul_lon}
			};

			// Inject ALL 12 Pushkara Points to catch Transiting Planets passing through them
			for (int r = 0; r < 12; r++) {
				string p_name = telugu_mode ? (get_rashi_name(r) + " పుష్కర భాగ (అదృష్ట బిందువు)") : ("Pushkara Luck Point of " + string(rashi_names[r]));
				blessing_points.push_back({p_name, (r * 30.0) + pushkara_degrees[r]});
			}

			// Add Natal Lifelong Blessings
			for (int p = 0; p <= 9; p++) {
				int rashi = planet_rashis[p];
				double deg_in_rashi = planet_lons[p] - (rashi * 30.0);
				if (std::abs(deg_in_rashi - pushkara_degrees[rashi]) <= 1.0) {
					string p_name = (p == 0) ? (telugu_mode ? "లగ్నం" : "Lagna") : get_planet_name(p);
					string bless_text = telugu_mode ? ("జన్మ " + p_name + " పుష్కర భాగలో ఉంది (జీవితకాల అదృష్టం)") : ("Natal " + p_name + " in PUSHKARA BHAGA (Lifelong Blessing)");
					blessing_points.push_back({bless_text, planet_lons[p]});
				}
			}
			if (telugu_mode) printf("--- జన్మ కుండలి శుభ బిందువులు (క్రాస్-చెకింగ్ కోసం) ---\n");
			else printf("--- NATAL AUSPICIOUS TARGETS (For Cross-Checking) ---\n");
			
			for (const auto& bp : blessing_points) {
				int rashi = ((int)(fmod(bp.lon, 360.0) / 30.0)) % 12;
				double rem = fmod(bp.lon, 30.0);
				int d = (int)rem;
				int m = (int)((rem - d) * 60.0);
				int s = (int)round((((rem - d) * 60.0) - m) * 60.0);
				if (s >= 60) { s -= 60; m += 1; }
				if (m >= 60) { m -= 60; d += 1; }
				if (d >= 30) { d -= 30; rashi = (rashi + 1) % 12; }
				
				printf(" * %-45s : %02d° %-7s %02d'%02d\"\n", bp.name.c_str(), d, get_rashi_name(rashi).c_str(), m, s);
			}
			printf("------------------------------------------------------------------------------------------------------------------------------------------\n");
			
			int t_benefics[] = {2, 4, 5, 6, yogi_planet_idx_ausp};
			bool blessing_triggered = false;
			
			vector<int> unique_benefics;
			for (int idx : t_benefics) {
				if (find(unique_benefics.begin(), unique_benefics.end(), idx) == unique_benefics.end()) {
					unique_benefics.push_back(idx);
				}
			}

			for (int m_idx : unique_benefics) {
				double t_lon;
				if (m_idx < 9) { swe_calc_ut(trans_jd, planets[m_idx-1], iflag, xx, serr); t_lon = xx[0]; } 
				else { swe_calc_ut(trans_jd, SE_TRUE_NODE, iflag, xx, serr); t_lon = fmod(xx[0] + 180.0, 360.0); }

				int t_rashi = (int)(t_lon / 30.0);
				double t_deg_in_rashi = t_lon - (t_rashi * 30.0);
				bool is_pushkara = (std::abs(t_deg_in_rashi - pushkara_degrees[t_rashi]) <= 1.0);
				string pushkara_flag = is_pushkara ? " [PUSHKARA: EXTREME LUCK DEGREE]" : "";

				for (const auto& bp : blessing_points) {
					double dist = std::abs(t_lon - bp.lon);
					if (dist > 180.0) dist = 360.0 - dist;

					if (dist <= master_orb) { 
						blessing_triggered = true;
						
						string severity;
						if (m_idx == 5) severity = "[!!! DIVINE BLESSING / EXPANSION !!!]";
						else if (m_idx == 6) severity = "[!!! WEALTH / HARMONY TRIGGER !!!]";
						else if (m_idx == yogi_planet_idx_ausp) severity = "[!!! YOGI PROSPERITY TRIGGER !!!]";
						else severity = "[EXACT DAY/TIME OPPORTUNITY]";
						
						string warning_type = "OPPORTUNITY";

						double e_in, e_peak, e_out;
						refine_bubble(m_idx, bp.lon, trans_jd, master_orb, e_in, e_peak, e_out);

						if (telugu_mode) {
							printf("  %s [%s] గోచార %s%s మీ %s ను తాకుతోంది\n", 
								   severity.c_str(), warning_type.c_str(), get_planet_name(m_idx).c_str(), pushkara_flag.c_str(), bp.name.c_str());
							printf("      -> [కాలక్రమం] ప్రవేశం: %s | గరిష్ఠం: %s | నిష్క్రమణ: %s (వ్యత్యాసం: %.2f°)\n", 
								   jd_to_string(e_in).c_str(), jd_to_string(e_peak).c_str(), jd_to_string(e_out).c_str(), dist);
						} else {
							printf("  %s [%s] Transit %s%s is hitting your %s\n", 
								   severity.c_str(), warning_type.c_str(), p_names_full[m_idx], pushkara_flag.c_str(), bp.name.c_str());
							printf("      -> [TIMELINE] Enter: %s | PEAK: %s | Exit: %s (Orb: %.2f°)\n", 
								   jd_to_string(e_in).c_str(), jd_to_string(e_peak).c_str(), jd_to_string(e_out).c_str(), dist);
						}                }
				}
			}

			if (!blessing_triggered) printf("  [System Clear] No major benefic exact degree collisions detected for this date.\n");
			printf("------------------------------------------------------------------------------------------------------------------------------------------\n");
		}  
    }
	void predict_marriage(int start_year, int end_year) {
        printf("\n=================================================================\n");
        printf("=== ADVANCED EVENT PREDICTION SCANNER (PROBABILITY CLUSTERING) ===\n");
        printf("=================================================================\n");
        printf("Scanning %d to %d for exact Jup/Sat/Sun/Ven/Dasha alignments...\n", start_year, end_year);
        
        // 1. Establish the Natal Anchors (Including Jaimini & D9)
        int l7_idx = 1; 
        int asc_rashi = planet_rashis[0];
        int h7_rashi = (asc_rashi + 6) % 12;
        for(int p=1; p<=7; p++) if(string(rashi_lords[h7_rashi]) == p_names_full[p]) l7_idx = p;
        
        int ul_rashi = get_arudha(12);
        int dk_idx = darakaraka_idx; // Extracted from Phase 4
        int dk_rashi = planet_rashis[dk_idx];

        int d9_asc = get_varga(9, planet_lons[0]);
        int d9_h7 = (d9_asc + 6) % 12;
        int d9_7L_idx = 1;
        for(int p=1; p<=7; p++) if(string(rashi_lords[d9_h7]) == p_names_full[p]) d9_7L_idx = p;
        int d9_7L_rashi = get_varga(9, planet_lons[d9_7L_idx]);

        // Map Dasha index (0=Ketu) to Planet index (1=Sun)
        int d_map[] = {9, 6, 1, 2, 3, 8, 5, 7, 4}; 
        
        double start_jd = swe_julday(start_year, 1, 1, 0.0, SE_GREG_CAL);
        double end_jd = swe_julday(end_year, 12, 31, 0.0, SE_GREG_CAL);

        struct Hit { int y, m, d; int score; string environment; string reason; };
        vector<Hit> peak_hits;

        // Sweep every single day in the timeframe
        for (double jd = start_jd; jd <= end_jd; jd += 1.0) {
            int score = 0;
            string reason = "";
            string environment = "General / Social setting";

            // 1. Dasha Readiness (Now includes DK)
            int md, ad; get_active_dasha_lords(jd, md, ad);
            int md_p = d_map[md]; int ad_p = d_map[ad];
            
            bool md_active = (md_p == l7_idx || md_p == 6 || md_p == 8 || md_p == 5 || planet_rashis[md_p] == ul_rashi || md_p == dk_idx);
            bool ad_active = (ad_p == l7_idx || ad_p == 6 || ad_p == 8 || ad_p == 5 || planet_rashis[ad_p] == ul_rashi || ad_p == dk_idx);
            
            if (md_active) score += 3;
            if (ad_active) score += 4;

            // Environment Inference (Based on active AD Lord's House)
            int ad_house = (planet_rashis[ad_p] - asc_rashi + 12) % 12 + 1;
            if (ad_house == 3 || ad_house == 4) environment = "Local area, neighborhood, or internet";
            else if (ad_house == 9) environment = "Travel, education, or spiritual setting";
            else if (ad_house == 10 || ad_house == 11) environment = "Workplace, professional network, or older friends";
            else if (ad_house == 2 || ad_house == 7) environment = "Family introduction or direct negotiation";

            // 2. Transit Jupiter Blessing (Now checks D1 AND D9 anchors)
            double xx_ju[6], xx_sa[6], xx_ve[6], xx_su[6]; char serr[256];
            swe_calc_ut(jd, SE_JUPITER, iflag, xx_ju, serr);
            int t_ju_rashi = (int)(xx_ju[0] / 30.0);
            auto ju_aspects = [&](int r) { int d = (r - t_ju_rashi + 12) % 12 + 1; return (d==1||d==5||d==7||d==9); };
            
            bool ju_hit_d1 = (ju_aspects(h7_rashi) || ju_aspects(planet_rashis[l7_idx]) || ju_aspects(ul_rashi) || ju_aspects(asc_rashi) || ju_aspects(dk_rashi));
            bool ju_hit_d9 = (ju_aspects(d9_asc) || ju_aspects(d9_7L_rashi)); // D9 Micro-karma trigger
            
            if (ju_hit_d1) score += 4;
            if (ju_hit_d9) { score += 2; reason += "[D9 Jup Hit] "; }

            // 3. Transit Saturn Commitment 
            swe_calc_ut(jd, SE_SATURN, iflag, xx_sa, serr);
            int t_sa_rashi = (int)(xx_sa[0] / 30.0);
            auto sa_aspects = [&](int r) { int d = (r - t_sa_rashi + 12) % 12 + 1; return (d==1||d==3||d==7||d==10); };
            
            bool sa_hit = (sa_aspects(h7_rashi) || sa_aspects(planet_rashis[l7_idx]) || sa_aspects(ul_rashi) || sa_aspects(asc_rashi) || sa_aspects(dk_rashi));
            if (sa_hit) score += 4;

            // Strict Filter: Event CANNOT trigger unless Dasha is ready AND Jup/Sat are locked
            if (ju_hit_d1 && sa_hit && (md_active || ad_active)) {
                
                // 4. Exact Month/Day Triggers (Sun & Venus)
                swe_calc_ut(jd, SE_VENUS, iflag, xx_ve, serr);
                int t_ve_rashi = (int)(xx_ve[0]/30.0);
                if (t_ve_rashi == h7_rashi || t_ve_rashi == ul_rashi || t_ve_rashi == asc_rashi || t_ve_rashi == planet_rashis[l7_idx] || t_ve_rashi == dk_rashi) {
                    score += 3; reason += "[Ven Exact Trigger] ";
                }
                
                swe_calc_ut(jd, SE_SUN, iflag, xx_su, serr);
                int t_su_rashi = (int)(xx_su[0]/30.0);
                if (t_su_rashi == h7_rashi || t_su_rashi == ul_rashi || t_su_rashi == asc_rashi || t_su_rashi == planet_rashis[l7_idx] || t_su_rashi == dk_rashi) {
                    score += 2; reason += "[Sun Exact Trigger] ";
                }

                // Threshold set to 18 out of 22 to capture high-probability clusters
                if (score >= 18) { 
                    int y, m, d; double jut;
                    swe_revjul(jd + (location.tz_offset/24.0), SE_GREG_CAL, &y, &m, &d, &jut);
                    
                    bool added = false;
                    for (auto& hit : peak_hits) {
                        if (hit.y == y && hit.m == m) {
                            if (score > hit.score) { hit.d = d; hit.score = score; hit.reason = reason; hit.environment = environment; }
                            added = true; break;
                        }
                    }
                    if (!added) {
                        char d_buf[64]; snprintf(d_buf, sizeof(d_buf), "[Dasha: %s/%s] ", dasha_lords[md], dasha_lords[ad]);
                        peak_hits.push_back({y, m, d, score, environment, string(d_buf) + reason});
                    }
                }
            }
        }

        // Output results
        if (peak_hits.empty()) {
            printf("No exact deterministic marriage clusters found in this timeframe.\n");
        } else {
            sort(peak_hits.begin(), peak_hits.end(), [](const Hit& a, const Hit& b) { return a.score > b.score; });
            
            printf("%-12s | %-8s | %-45s | %s\n", "Window", "Power", "Likely Environment", "Astrological Triggers");
            printf("--------------------------------------------------------------------------------------------------------------------------------------\n");
            int count = 0;
            for (const auto& hit : peak_hits) {
                if (count++ >= 10) break;
                printf("%02d/%02d/%04d | %d/22 pts | %-45s | %s\n", hit.d, hit.m, hit.y, hit.score, hit.environment.c_str(), hit.reason.c_str());
            }
        }
        printf("=================================================================\n");
    }
	
    void finalize_json() { json_output += "}"; printf("%s\n", json_output.c_str()); }
// Helper to safely access array strings
string safe_str(const string* arr, int idx, int size) {
    if (idx < 0 || idx >= size) return "Unknown";
    return arr[idx];
}

void export_web_json(int t_year, int t_month, int t_day) {
    json root;

    // 1. Birth Chart (Renamed key to match JS: tab_birth)
    root["tab_birth"]["lagna"] = rashi_names[planet_rashis[0]];
    root["tab_birth"]["lagna_degree"] = fmod(planet_lons[0], 30.0);
    
    json planets = json::array();
    for (int i = 1; i <= 9; i++) {
        planets.push_back({
            {"name", p_names_full[i]},
            {"rashi", rashi_names[planet_rashis[i]]},
            {"degree", fmod(planet_lons[i], 30.0)},
            {"nakshatra", nak_names[(int)(planet_lons[i] / (360.0 / 27.0))]}
        });
    }
    root["tab_birth"]["planets"] = planets;

    // 2. Mangal Dosha (Renamed key to match JS: tab_dosha)
    int ma_h_lagna = (planet_rashis[3] - planet_rashis[0] + 12) % 12 + 1;
    root["tab_dosha"]["mangal_dosha"] = (ma_h_lagna==1 || ma_h_lagna==2 || ma_h_lagna==4 || ma_h_lagna==7 || ma_h_lagna==8 || ma_h_lagna==12);
    root["tab_dosha"]["analysis"] = "Mangal Dosha calculation complete.";

    // 3. Dasha (Renamed key to match JS: tab_dasha)
    int md, ad;
    double target_jd = swe_julday(t_year, t_month, t_day, 12.0 - location.tz_offset, SE_GREG_CAL);
    get_active_dasha_lords(target_jd, md, ad);
    root["tab_dasha"]["mahadasha"] = dasha_lords[md];
    root["tab_dasha"]["antardasha"] = dasha_lords[ad];

    cout << root.dump(4) << endl; 
}

};

void calculate_synastry(const JyotishaEngine& p1, const JyotishaEngine& p2) {
    bool te = p1.telugu_mode; // Pull the language state from Person 1
    if (!p1.av_calculated) const_cast<JyotishaEngine&>(p1).calculate_ashtakavarga(true);
    if (!p2.av_calculated) const_cast<JyotishaEngine&>(p2).calculate_ashtakavarga(true);

    if (te) {
        printf("\n=================================================================\n");
        printf("=== V8.4 వధూవరుల జాతక పొంతన & దోష పరిహార నివేదిక ===\n");
        printf("=================================================================\n");
    } else {
        printf("\n=================================================================\n");
        printf("=== V8.4 ULTIMATE SOULMATE & DOSHA RECTIFICATION AUDIT ===\n");
        printf("=================================================================\n");
    }

    auto get_house = [](int p_rashi, int asc_rashi) { return (p_rashi - asc_rashi + 12) % 12 + 1; };
    auto is_kd_house = [](int h) { return h==1||h==2||h==4||h==7||h==8||h==12; };
    auto get_nodal_dosha = [&](int ra_rashi, int asc_rashi) {
        int h_ra = get_house(ra_rashi, asc_rashi);
        int h_ke = get_house((ra_rashi + 6) % 12, asc_rashi);
        return (h_ra==1||h_ra==2||h_ra==7||h_ra==8 || h_ke==1||h_ke==2||h_ke==7||h_ke==8);
    };
    auto check_dist = [](double l1, double l2, double orb) { 
        double d = std::abs(fmod(l1 - l2 + 360.0, 360.0)); 
        return (d <= orb || d >= 360.0 - orb) || (std::abs(d - 180.0) <= orb); 
    };
    auto get_midpoint = [](double l1, double l2) {
        double diff = std::abs(l1 - l2);
        if (diff > 180.0) {
            double mid = fmod((l1 + l2 + 360.0) / 2.0, 360.0);
            return fmod(mid + 180.0, 360.0);
        }
        return fmod((l1 + l2) / 2.0, 360.0);
    };
    auto is_pushkara = [](int d9_rashi) {
        return (d9_rashi == 1 || d9_rashi == 3 || d9_rashi == 6 || d9_rashi == 8 || d9_rashi == 11);
    };

    int gana_arr[27] = {0,1,2,1,0,1,0,0,2,2,1,1,0,2,0,2,0,2,2,1,1,0,2,2,1,1,0}; 
    int yoni_arr[27] = {1,2,3,4,4,5,6,7,6,8,8,9,10,11,10,11,12,12,5,13,14,13,15,1,15,9,2}; 
    int nadi_arr[27] = {0,1,2,2,1,0,0,1,2,2,1,0,0,1,2,2,1,0,0,1,2,2,1,0,0,1,2};
    int varna_arr[12] = {1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0}; 
    int vashya_arr[12] = {1, 1, 0, 2, 3, 0, 0, 4, 1, 2, 0, 2}; 
    int vedha_map[27] = {17, 16, 15, 14, 13, 21, 20, 19, 18, 26, 25, 24, 23, 4, 3, 2, 1, 0, 8, 7, 6, 5, -1, 12, 11, 10, 9};
    int rajju_arr[27] = {0,1,2,3,4, 3,2,1,0, 0,1,2,3,4, 3,2,1,0, 0,1,2,3,4, 3,2,1,0};

    // --- Core Variables Person 1 ---
    int p1_asc = p1.planet_rashis[0], p1_su = p1.planet_rashis[1], p1_mo = p1.planet_rashis[2];
    int p1_ma = p1.planet_rashis[3], p1_ju = p1.planet_rashis[5], p1_ve = p1.planet_rashis[6];
    int p1_sa = p1.planet_rashis[7], p1_ra = p1.planet_rashis[8], p1_ke = p1.planet_rashis[9];
    int p1_7L = 1, p1_5L = 1, p1_9L = 1; 
    for(int p=1; p<=7; p++) {
        if(string(rashi_lords[(p1_asc+6)%12]) == p_names_full[p]) p1_7L = p;
        if(string(rashi_lords[(p1_asc+4)%12]) == p_names_full[p]) p1_5L = p;
        if(string(rashi_lords[(p1_asc+8)%12]) == p_names_full[p]) p1_9L = p;
    }
    int p1_ul = p1.get_arudha(12), p1_a7 = p1.get_arudha(7);
    int p1_d9_asc = p1.get_varga(9, p1.planet_lons[0]);
    int p1_d9_ve = p1.get_varga(9, p1.planet_lons[6]);
    int p1_d30_7L = p1.get_varga(30, p1.planet_lons[p1_7L]);
    int p1_d60_7L = p1.get_varga(60, p1.planet_lons[p1_7L]);
    double p1_bb = get_midpoint(p1.planet_lons[8], p1.planet_lons[2]);

    // --- Core Variables Person 2 ---
    int p2_asc = p2.planet_rashis[0], p2_su = p2.planet_rashis[1], p2_mo = p2.planet_rashis[2];
    int p2_ma = p2.planet_rashis[3], p2_ju = p2.planet_rashis[5], p2_ve = p2.planet_rashis[6];
    int p2_sa = p2.planet_rashis[7], p2_ra = p2.planet_rashis[8], p2_ke = p2.planet_rashis[9];
    int p2_7L = 1, p2_5L = 1, p2_9L = 1; 
    for(int p=1; p<=7; p++) {
        if(string(rashi_lords[(p2_asc+6)%12]) == p_names_full[p]) p2_7L = p;
        if(string(rashi_lords[(p2_asc+4)%12]) == p_names_full[p]) p2_5L = p;
        if(string(rashi_lords[(p2_asc+8)%12]) == p_names_full[p]) p2_9L = p;
    }
    int p2_ul = p2.get_arudha(12), p2_a7 = p2.get_arudha(7);
    int p2_d9_asc = p2.get_varga(9, p2.planet_lons[0]);
    int p2_d9_ve = p2.get_varga(9, p2.planet_lons[6]);
    int p2_d30_7L = p2.get_varga(30, p2.planet_lons[p2_7L]);
    int p2_d60_7L = p2.get_varga(60, p2.planet_lons[p2_7L]);
    double p2_bb = get_midpoint(p2.planet_lons[8], p2.planet_lons[2]);

    int total_score = 0;

    int nak_1 = (int)(p1.moon_lon / (360.0 / 27.0)); 
    int nak_2 = (int)(p2.moon_lon / (360.0 / 27.0));

    if (te) {
        printf("[జన్మ రాశి & నక్షత్ర వివరాలు]\n");
        printf("వ్యక్తి 1 చంద్రుడు: %02d° %s | నక్షత్రం: %s (%d)\n", (int)fmod(p1.moon_lon, 30.0), te_rashi_names[p1_mo], te_nak_names[nak_1], nak_1 + 1);
        printf("వ్యక్తి 2 చంద్రుడు: %02d° %s | నక్షత్రం: %s (%d)\n", (int)fmod(p2.moon_lon, 30.0), te_rashi_names[p2_mo], te_nak_names[nak_2], nak_2 + 1);
    } else {
        printf("[LUMINAL ANCHORS]\n");
        printf("Person 1 Moon: %02d° %s | Nakshatra: %s (%d)\n", (int)fmod(p1.moon_lon, 30.0), rashi_names[p1_mo], nak_names[nak_1], nak_1 + 1);
        printf("Person 2 Moon: %02d° %s | Nakshatra: %s (%d)\n", (int)fmod(p2.moon_lon, 30.0), rashi_names[p2_mo], nak_names[nak_2], nak_2 + 1);
    }

    // =================================================================
    // 1. ATTRACTION & CHEMISTRY (29 Points Max)
    // =================================================================
    if (te) printf("\n[1. ఆకర్షణ & అనుబంధం - శృంగారం & ఆకర్షణ శక్తి (29 pts)]\n");
    else printf("\n[1. ATTRACTION & CHEMISTRY - Romance & Magnetism (29 pts)]\n");
    
    int s_vema = 0, s_sumo = 0, s_vemo = 0, s_yoni = 0, s_varna = 0, s_vashya = 0;

    bool v1_m2 = (get_house(p1_ve, p2_ma)==1 || get_house(p1_ve, p2_ma)==7);
    bool m1_v2 = (get_house(p1_ma, p2_ve)==1 || get_house(p1_ma, p2_ve)==7);
    if (v1_m2 && m1_v2) { s_vema = 8; printf(te ? "  + 8 pts | శుక్ర-కుజ బంధం   : డబుల్ లాక్ (పరస్పర ఆకర్షణ/దృష్టి)\n" : "  + 8 pts | Venus-Mars    : DOUBLE LOCK (Mutual cross-conjunction/opposition)\n"); }
    else if (v1_m2 || m1_v2) { s_vema = 6; printf(te ? "  + 6 pts | శుక్ర-కుజ బంధం   : ఆకర్షణీయం (అయస్కాంత శక్తి)\n" : "  + 6 pts | Venus-Mars    : MAGNETIC (Conjunction/Opposition creates undeniable spark)\n"); }
    else { s_vema = 2; printf(te ? "  + 2 pts | శుక్ర-కుజ బంధం   : సాధారణం (సాధారణ శారీరక ఆకర్షణ)\n" : "  + 2 pts | Venus-Mars    : AVERAGE (Standard physical chemistry)\n"); }

    bool su1_mo2 = (get_house(p1_su, p2_mo)==1 || get_house(p1_su, p2_mo)==5 || get_house(p1_su, p2_mo)==9 || get_house(p1_su, p2_mo)==7);
    bool su2_mo1 = (get_house(p2_su, p1_mo)==1 || get_house(p2_su, p1_mo)==5 || get_house(p2_su, p1_mo)==9 || get_house(p2_su, p1_mo)==7);
    if (su1_mo2 && su2_mo1) { s_sumo = 8; printf(te ? "  + 8 pts | సూర్య-చంద్ర బంధం : డబుల్ లాక్ (పరస్పర అహం-భావోద్వేగాల అవగాహన)\n" : "  + 8 pts | Sun-Moon      : DOUBLE LOCK (Mutual ego-emotion mirroring)\n"); }
    else if (su1_mo2 || su2_mo1) { s_sumo = 6; printf(te ? "  + 6 pts | సూర్య-చంద్ర బంధం : సామరస్యం (సూర్యుడు భాగస్వామి చంద్రుడిని ప్రకాశవంతం చేస్తాడు)\n" : "  + 6 pts | Sun-Moon      : HARMONY (Sun illuminates partner's Moon)\n"); }
    else { s_sumo = 2; printf(te ? "  + 2 pts | సూర్య-చంద్ర బంధం : సాధారణం\n" : "  + 2 pts | Sun-Moon      : AVERAGE (Standard luminary interaction)\n"); }

    bool v1_mo2 = (get_house(p1_ve, p2_mo)==1 || get_house(p1_ve, p2_mo)==5 || get_house(p1_ve, p2_mo)==9);
    bool v2_mo1 = (get_house(p2_ve, p1_mo)==1 || get_house(p2_ve, p1_mo)==5 || get_house(p2_ve, p1_mo)==9);
    if (v1_mo2 || v2_mo1) { s_vemo = 5; printf(te ? "  + 5 pts | శుక్ర-చంద్ర బంధం : మధురానుభూతి (లోతైన ఆప్యాయత, భావోద్వేగ బంధం)\n" : "  + 5 pts | Venus-Moon    : SWEETNESS (Deep affection and emotional bonding)\n"); }
    else { s_vemo = 2; printf(te ? "  + 2 pts | శుక్ర-చంద్ర బంధం : తటస్థం\n" : "  + 2 pts | Venus-Moon    : NEUTRAL (Affection requires conscious effort)\n"); }

    if (yoni_arr[nak_1] == yoni_arr[nak_2]) { s_yoni = 4; printf(te ? "  + 4 pts | యోని కూటమి     : ఏక యోని (పరిపూర్ణ శారీరక అనుకూలత)\n" : "  + 4 pts | Yoni Kuta     : IDENTICAL (Perfect instinctual/sexual rhythm)\n"); }
    else { s_yoni = 2; printf(te ? "  + 2 pts | యోని కూటమి     : ఆమోదయోగ్యం\n" : "  + 2 pts | Yoni Kuta     : ACCEPTABLE (Different animal symbols)\n"); }

    if (varna_arr[p1_mo] <= varna_arr[p2_mo]) { s_varna = 2; printf(te ? "  + 2 pts | వర్ణ కూటమి     : అనుకూలం (ఆధ్యాత్మిక అహంకారం సమతుల్యం)\n" : "  + 2 pts | Varna Kuta    : COMPATIBLE (Spiritual ego aligns)\n"); }
    else { s_varna = 1; printf(te ? "  + 1 pts | వర్ణ కూటమి     : స్వల్ప ఘర్షణ (ఆధ్యాత్మిక అహంకార వివాదం)\n" : "  + 1 pts | Varna Kuta    : FRICTION (Minor spiritual ego conflict)\n"); }

    if (vashya_arr[p1_mo] == vashya_arr[p2_mo]) { s_vashya = 2; printf(te ? "  + 2 pts | వశ్య కూటమి     : సమానం (ఎవరు ఎవరిపైనా ఆధిపత్యం చెలాయించరు)\n" : "  + 2 pts | Vashya Kuta   : EQUAL (Neither dominates the other)\n"); }
    else { s_vashya = 1; printf(te ? "  + 1 pts | వశ్య కూటమి     : అసమతుల్యం (ఒకరు సహజంగా మరొకరిని నియంత్రిస్తారు)\n" : "  + 1 pts | Vashya Kuta   : UNBALANCED (One sign naturally controls the other)\n"); }
    
    total_score += (s_vema + s_sumo + s_vemo + s_yoni + s_varna + s_vashya);

    // =================================================================
    // 2. PSYCHOLOGICAL HARMONY & SAV RESONANCE (33 Points Max)
    // =================================================================
    if (te) printf("\n[2. మానసిక సామరస్యం - మనస్సు & సౌఖ్యం (33 pts)]\n");
    else printf("\n[2. PSYCHOLOGICAL HARMONY - Mind & Comfort (33 pts)]\n");
    
    int s_bha = 0, s_nadi = 0, s_gana = 0, s_tara = 0, s_vedha = 0, s_maitri = 0, s_sav = 0;

    int mo_mo = get_house(p2_mo, p1_mo); 
    if (mo_mo==6||mo_mo==8||mo_mo==2||mo_mo==12) { s_bha = 0; printf(te ? "  + 0 pts | భకూట దోషం      : దోషం (6/8 లేదా 2/12 స్థానాలు తీవ్ర ఘర్షణను సృష్టిస్తాయి)\n" : "  + 0 pts | Bhakoot (Sign): DOSHA (6/8 or 2/12 creates deep friction)\n"); }
    else { s_bha = 6; printf(te ? "  + 6 pts | భకూట స్థానం    : శుభకరం (భావోద్వేగ రక్షణ మరియు ప్రశాంతత)\n" : "  + 6 pts | Bhakoot (Sign): AUSPICIOUS (Emotional safety and flow)\n"); }

    if (nadi_arr[nak_1] == nadi_arr[nak_2]) { s_nadi = 2; printf(te ? "  + 2 pts | నాడి కూటమి     : ఏక నాడి దోషం (ఒకే రకమైన నాడీ వ్యవస్థ)\n" : "  + 2 pts | Nadi (Pulse)  : SAME NADI (Traditional Dosha, implies identical soul frequency)\n"); }
    else { s_nadi = 6; printf(te ? "  + 6 pts | నాడి కూటమి     : అత్యుత్తమం (పరస్పర అనుకూలమైన నాడీ వ్యవస్థలు)\n" : "  + 6 pts | Nadi (Pulse)  : EXCELLENT (Complementary nervous systems)\n"); }

    if (gana_arr[nak_1] == gana_arr[nak_2]) { s_gana = 5; printf(te ? "  + 5 pts | గణ కూటమి       : అనుకూలం (ఒకే రకమైన స్వభావం)\n" : "  + 5 pts | Gana Kuta     : HARMONIOUS (Same temperament category)\n"); }
    else { s_gana = 1; printf(te ? "  + 1 pts | గణ కూటమి       : విభేదం (భిన్నమైన స్వభావాలు)\n" : "  + 1 pts | Gana Kuta     : CLASH (Different temperaments)\n"); }

    int tara_dist = (nak_2 - nak_1 + 27) % 9;
    if (tara_dist==1||tara_dist==3||tara_dist==5||tara_dist==7||tara_dist==8) { 
        s_tara = 3; printf(te ? "  + 3 pts | తారా బలం       : శుభకరం (సంపత్/క్షేమ/సాధక/మిత్ర)\n" : "  + 3 pts | Tara Kuta     : AUSPICIOUS (Sampat/Kshema/Sadhaka/Mitra)\n"); 
    } else if (tara_dist==4) { 
        s_tara = 1; printf(te ? "  + 1 pts | తారా బలం       : ప్రత్యక్ తార (కర్మ సంబంధిత అడ్డంకులు)\n" : "  + 1 pts | Tara Kuta     : PRATYAK (Obstacles / Karmic debt-clearing love)\n"); 
    } else { 
        s_tara = 0; printf(te ? "  + 0 pts | తారా బలం       : ప్రతికూలం (విపత్/వధ/జన్మ తారలు)\n" : "  + 0 pts | Tara Kuta     : CHALLENGING (Vipat / Vadha / Janma)\n"); 
    }

    bool vedha_hit = false;
    int mals[] = {1, 3, 7, 8, 9}; 
    for (int m : mals) {
        if ((int)(p2.planet_lons[m] / (360.0/27.0)) == vedha_map[nak_1]) vedha_hit = true;
        if ((int)(p1.planet_lons[m] / (360.0/27.0)) == vedha_map[nak_2]) vedha_hit = true;
    }
    if (vedha_hit) { s_vedha = 0; printf(te ? "  + 0 pts | వేధ దోషం       : బాధాకరం (పాప గ్రహాలచే వేధ)\n" : "  + 0 pts | SBC Vedha     : AFFLICTED (Malefics cast Vedha on Moon Nakshatra)\n"); }
    else { s_vedha = 2; printf(te ? "  + 2 pts | వేధ దోషం       : సురక్షితం (చంద్రునిపై ఎలాంటి వేధ లేదు)\n" : "  + 2 pts | SBC Vedha     : CLEAR (No Malefic Vedha on Moon)\n"); }

    int m_lord1=1, m_lord2=1;
    for(int p=1; p<=7; p++) { if(string(rashi_lords[p1_mo])==p_names_full[p]) m_lord1=p; if(string(rashi_lords[p2_mo])==p_names_full[p]) m_lord2=p; }
    int maitri_grid[8][8] = { {0,0,0,0,0,0,0,0}, {0,2,2,2,1,2,0,0}, {0,2,2,1,2,1,1,1}, {0,2,2,2,0,2,1,1}, {0,2,0,1,2,1,2,1}, {0,2,2,2,0,2,0,1}, {0,0,0,1,2,1,2,2}, {0,0,0,0,2,1,2,2} };
    int m1 = maitri_grid[m_lord1][m_lord2], m2 = maitri_grid[m_lord2][m_lord1];
    if(m1==2&&m2==2) { s_maitri=5; printf(te ? "  + 5 pts | గ్రహ మైత్రి     : అత్యుత్తమం (పరస్పర గ్రహ మిత్రులు)\n" : "  + 5 pts | Graha Maitri  : EXCELLENT (Mutual planetary friends)\n"); }
    else if((m1==2&&m2==1)||(m1==1&&m2==2)) { s_maitri=4; printf(te ? "  + 4 pts | గ్రహ మైత్రి     : మంచిది (మిత్రుడు / తటస్థం)\n" : "  + 4 pts | Graha Maitri  : GOOD (Friend / Neutral)\n"); }
    else if(m1==1&&m2==1) { s_maitri=3; printf(te ? "  + 3 pts | గ్రహ మైత్రి     : సాధారణం (పరస్పర తటస్థం)\n" : "  + 3 pts | Graha Maitri  : AVERAGE (Mutual Neutral)\n"); }
    else if((m1==2&&m2==0)||(m1==0&&m2==2)) { s_maitri=2; printf(te ? "  + 2 pts | గ్రహ మైత్రి     : సవాలు (మిత్రుడు / శత్రువు)\n" : "  + 2 pts | Graha Maitri  : CHALLENGING (Friend / Enemy)\n"); }
    else { s_maitri=1; printf(te ? "  + 1 pts | గ్రహ మైత్రి     : శత్రుత్వం (పరస్పర శత్రువులు)\n" : "  + 1 pts | Graha Maitri  : FRICTION (Mutual Enemies or Neutral/Enemy)\n"); }

    int sav_p1_in_p2 = p2.sav_scores[p1_mo];
    int sav_p2_in_p1 = p1.sav_scores[p2_mo];
    if(sav_p1_in_p2 >= 28) s_sav += 3; else if(sav_p1_in_p2 >= 25) s_sav += 1;
    if(sav_p2_in_p1 >= 28) s_sav += 3; else if(sav_p2_in_p1 >= 25) s_sav += 1;
    printf(te ? "  + %d pts | SAV అనుకూలత    : పరస్పర పోషణ (P1 in P2: %d, P2 in P1: %d)\n" : "  + %d pts | SAV Resonance : MUTUAL NOURISHMENT (P1 in P2: %d, P2 in P1: %d)\n", s_sav, sav_p1_in_p2, sav_p2_in_p1);

    total_score += (s_bha + s_nadi + s_gana + s_tara + s_vedha + s_maitri + s_sav);

    // =================================================================
    // 3. KARMIC DESTINY & INTENSITY (42 Points Max)
    // =================================================================
    if (te) printf("\n[3. కర్మ బంధం - ఆత్మల కలయిక (42 pts)]\n");
    else printf("\n[3. KARMIC DESTINY - Soul Binding & Exact Overlays (42 pts)]\n");
    
    int s_nodal = 0, s_ul = 0, s_akdk = 0, s_bb = 0, s_d60_dig = 0, s_d60_mut = 0, s_ascmo = 0;

    bool asc1_mo2 = (get_house(p1_asc, p2_mo)==1 || get_house(p1_asc, p2_mo)==7);
    bool asc2_mo1 = (get_house(p2_asc, p1_mo)==1 || get_house(p2_asc, p1_mo)==7);
    if (asc1_mo2 && asc2_mo1) { s_ascmo = 8; printf(te ? "  + 8 pts | లగ్న-చంద్ర బంధం : ద్వంద్వ ఆత్మ బంధం (పరస్పర అనుసంధానం)\n" : "  + 8 pts | Lagna-Moon    : DOUBLE SOUL TIE (Mutual Ascendant/Moon locks)\n"); }
    else if (asc1_mo2 || asc2_mo1) { s_ascmo = 6; printf(te ? "  + 6 pts | లగ్న-చంద్ర బంధం : ఆత్మ బంధం (భాగస్వామి చంద్రుడు లగ్నంపై పడటం)\n" : "  + 6 pts | Lagna-Moon    : SOUL TIE (Partner's Moon on Ascendant axis)\n"); }
    else { s_ascmo = 0; printf(te ? "  + 0 pts | లగ్న-చంద్ర బంధం : స్వతంత్రం (ప్రత్యక్ష లగ్న-చంద్ర సంబంధం లేదు)\n" : "  + 0 pts | Lagna-Moon    : INDEPENDENT (No direct Ascendant-Moon overlay)\n"); }

    bool mo_node_hit = (p1_ra==p2_mo || p1_ke==p2_mo || p2_ra==p1_mo || p2_ke==p1_mo);
    bool nodal_hit = false;
    int n1[] = {p1_ra, p1_ke}; int t2[] = {p2_su, p2_mo, p2_ve, p2_asc, p2.planet_rashis[p2_7L]};
    for(int n : n1) for(int t : t2) if(n == t) nodal_hit = true;
    int n2[] = {p2_ra, p2_ke}; int t1[] = {p1_su, p1_mo, p1_ve, p1_asc, p1.planet_rashis[p1_7L]};
    for(int n : n2) for(int t : t1) if(n == t) nodal_hit = true;

    if (mo_node_hit) { s_nodal = 10; printf(te ? "  +10 pts | రాహు/కేతు పట్టు : కర్మ రుణం (చంద్రుడు కచ్చితంగా నోడల్ యాక్సిస్‌పై ఉన్నాడు - తీవ్రత ఎక్కువ)\n" : "  +10 pts | Nodal Grip    : KARMIC DEBT (Moon exactly on Nodal Axis - High intensity)\n"); }
    else if (nodal_hit) { s_nodal = 6; printf(te ? "  + 6 pts | రాహు/కేతు పట్టు : తీవ్రమైన బంధం (రాహు/కేతువులు భాగస్వామి ముఖ్య స్థానాలతో కలయిక)\n" : "  + 6 pts | Nodal Grip    : INTENSE (Rahu/Ketu conjunct partner's core pillars)\n"); }
    else { s_nodal = 2; printf(te ? "  + 2 pts | రాహు/కేతు పట్టు : సురక్షితం (భారీ కర్మ రుణం లేదు)\n" : "  + 2 pts | Nodal Grip    : CLEAR (No heavy karmic debt or nodal obsession)\n"); }

    if (p1_ul == p2_ul || p1_a7 == p2_mo || p1_a7 == p2_ve || p2_a7 == p1_mo || p2_a7 == p1_ve) {
        s_ul = 7; printf(te ? "  + 7 pts | ఆరూఢ లగ్న బంధం : విధి నిర్ణయం (దారపదం కచ్చితంగా భాగస్వామిపై పడటం)\n" : "  + 7 pts | Arudha (A7/UL): FATED (Darapada exactly hits spouse markers)\n");
    } else { s_ul = 2; printf(te ? "  + 2 pts | ఆరూఢ లగ్న బంధం : సాధారణం\n" : "  + 2 pts | Arudha (A7/UL): STANDARD (No direct Arudha overlay)\n"); }

    double ak_1 = p1.planet_lons[p1.atmakaraka_idx], dk_2 = p2.planet_lons[p2.darakaraka_idx];
    double dk_1 = p1.planet_lons[p1.darakaraka_idx], mo_2 = p2.planet_lons[2];
    if (check_dist(ak_1, dk_2, 5.0) || check_dist(p2.planet_lons[p2.atmakaraka_idx], p1.planet_lons[p1.darakaraka_idx], 5.0)) {
        s_akdk = 8; printf(te ? "  + 8 pts | డిగ్రీల కలయిక   : ఆత్మల ఒప్పందం (AK మరియు DK 5° లోపు కలయిక)\n" : "  + 8 pts | Exact Degrees : SOUL CONTRACT (AK conjunct DK within 5° orb)\n");
    } else if (check_dist(dk_1, mo_2, 8.0) || check_dist(dk_2, p1.planet_lons[2], 8.0)) {
        s_akdk = 6; printf(te ? "  + 6 pts | డిగ్రీల కలయిక   : సహజ భాగస్వామి (DK భాగస్వామి చంద్రునితో కలయిక)\n" : "  + 6 pts | Exact Degrees : DEFAULT SPOUSE (DK conjunct partner's Moon within orb)\n");
    } else { s_akdk = 2; printf(te ? "  + 2 pts | డిగ్రీల కలయిక   : స్వతంత్రం\n" : "  + 2 pts | Exact Degrees : INDEPENDENT (No exact Jaimini degree locks)\n"); }

    if (check_dist(p1_bb, p2.planet_lons[6], 3.0) || check_dist(p1_bb, p2.planet_lons[p2_7L], 3.0) ||
        check_dist(p2_bb, p1.planet_lons[6], 3.0) || check_dist(p2_bb, p1.planet_lons[p1_7L], 3.0)) {
        s_bb = 4; printf(te ? "  + 4 pts | భృగు బిందు     : విధి ప్రేరేపితం (డెస్టినీ పాయింట్ కచ్చితంగా భాగస్వామి 7వ అధిపతి/శుక్రునితో కలయిక)\n" : "  + 4 pts | Bhrigu Bindu  : FATED TRIGGER (Destiny point exactly conjunct/opposes partner's 7L/Venus)\n");
    } else { s_bb = 0; printf(te ? "  + 0 pts | భృగు బిందు     : తటస్థం\n" : "  + 0 pts | Bhrigu Bindu  : SILENT (No exact destiny point triggers detected)\n"); }

    int ex_signs[] = {-1, 0, 1, 9, 5, 3, 11, 6}; 
    int own_1[] = {-1, 4, 3, 0, 2, 8, 1, 9};
    int own_2[] = {-1, -1, -1, 7, 5, 11, 6, 10};
    bool d60_p1_strong = (p1_d60_7L == ex_signs[p1_7L] || p1_d60_7L == own_1[p1_7L] || p1_d60_7L == own_2[p1_7L]);
    bool d60_p2_strong = (p2_d60_7L == ex_signs[p2_7L] || p2_d60_7L == own_1[p2_7L] || p2_d60_7L == own_2[p2_7L]);
    
    if (d60_p1_strong && d60_p2_strong) { s_d60_dig = 3; printf(te ? "  + 3 pts | D60 7L బలం     : శాశ్వత బంధం (ఇద్దరి D60 7వ అధిపతులు అత్యంత బలంగా ఉన్నారు)\n" : "  + 3 pts | D60 7L Dignity: ETERNAL BOND (Both D60 7th Lords hold massive dignity across lifetimes)\n"); }
    else if (d60_p1_strong || d60_p2_strong) { s_d60_dig = 1; printf(te ? "  + 1 pts | D60 7L బలం     : పాక్షిక బలం (ఒకరి D60 7వ అధిపతి బలంగా ఉన్నాడు)\n" : "  + 1 pts | D60 7L Dignity: PARTIAL PROMISE (One D60 7th Lord shows past-life marital mastery)\n"); }
    else { s_d60_dig = 0; printf(te ? "  + 0 pts | D60 7L బలం     : సాధారణం\n" : "  + 0 pts | D60 7L Dignity: STANDARD (D60 does not show exalted marital karma)\n"); }

    if (get_house(p1_d60_7L, p2_d60_7L) == 1 || get_house(p1_d60_7L, p2_d60_7L) == 7) {
        s_d60_mut = 2; printf(te ? "  + 2 pts | D60 పరస్పర బంధం : విడదీయరాని బంధం (D60 7వ అధిపతులు పరస్పర కలయిక/దృష్టి)\n" : "  + 2 pts | D60 Mutual    : UNBREAKABLE (D60 7th Lords conjunct/opposed across lifetimes)\n");
    } else { s_d60_mut = 0; printf(te ? "  + 0 pts | D60 పరస్పర బంధం : స్వతంత్రం\n" : "  + 0 pts | D60 Mutual    : INDEPENDENT (No D60 mutual aspect)\n"); }

    total_score += (s_ascmo + s_nodal + s_ul + s_akdk + s_bb + s_d60_dig + s_d60_mut);

    // =================================================================
    // 4. MARRIAGE STABILITY & DHARMA (31 Points Max)
    // =================================================================
    if (te) printf("\n[4. వివాహ స్థిరత్వం & ధర్మం - ఆయుష్షు & లక్ష్యం (31 pts)]\n");
    else printf("\n[4. MARRIAGE STABILITY & DHARMA - Longevity & Purpose (31 pts)]\n");
    
    int s_sat = 0, s_7L = 0, s_kuja = 0, s_comp = 0, s_d30 = 0, s_pushkara = 0, s_nodal_dosha = 0, s_rajju = 0;

    if (p1_sa==p2_mo || p1_sa==p2_ve || p1_sa==p2_ul || p1_sa==p2.planet_rashis[p2_7L] ||
        p2_sa==p1_mo || p2_sa==p1_ve || p2_sa==p1_ul || p2_sa==p1.planet_rashis[p1_7L]) {
        s_sat = 6; printf(te ? "  + 6 pts | శని బంధం       : దృఢత్వం (శని భాగస్వామి వివాహ స్థానాలను బలంగా పట్టుకున్నాడు)\n" : "  + 6 pts | Saturn Binding: GRAVITY (Saturn grips partner's marriage markers)\n");
    } else { s_sat = 2; printf(te ? "  + 2 pts | శని బంధం       : సాధారణం (భారీ శని బంధం లేదు)\n" : "  + 2 pts | Saturn Binding: LIGHT (Lacks heavy Saturnian glue)\n"); }

    bool parivartana = (get_house(p1.planet_rashis[p1_7L], p2_asc) == get_house(p2.planet_rashis[p2_7L], p1_asc));
    if (parivartana && (p1_7L==p2_5L || p1_7L==p2_9L || p2_7L==p1_5L || p2_7L==p1_9L)) {
        s_7L = 8; printf(te ? "  + 8 pts | భావాధిపతుల బంధం : కళానిధి యోగం (దోషరహిత పరస్పర మార్పిడి మరియు త్రికోణ కలయిక)\n" : "  + 8 pts | House Lords   : KALANIDHI YOGA (Flawless mutual exchange and Trinal crossing)\n");
    } else if (p1_7L==p2_5L || p1_7L==p2_9L || p2_7L==p1_5L || p2_7L==p1_9L || p1_5L==p2_ve || p2_5L==p1_ve || get_house(p1.planet_rashis[p1_5L], p2_ve)==1 || get_house(p2.planet_rashis[p2_5L], p1_ve)==1) {
        s_7L = 6; printf(te ? "  + 6 pts | భావాధిపతుల బంధం : ధార్మిక బంధం (5/9 అధిపతులు 7వ/శుక్రునితో కలయిక - పూర్వ పుణ్యం)\n" : "  + 6 pts | House Lords   : DHARMIC (5th/9th lords cross-connect with 7th/Venus - Poorva Punya)\n");
    } else { s_7L = 2; printf(te ? "  + 2 pts | భావాధిపతుల బంధం : సాధారణం\n" : "  + 2 pts | House Lords   : AVERAGE (No major Trinal cross-chart exchanges)\n"); }

    bool kd1 = is_kd_house(get_house(p1_ma, p1_asc)) || is_kd_house(get_house(p1_ma, p1_mo));
    bool kd2 = is_kd_house(get_house(p2_ma, p2_asc)) || is_kd_house(get_house(p2_ma, p2_mo));
    if (kd1 == kd2) { s_kuja = 4; printf(te ? "  + 4 pts | కుజ దోషం       : అద్భుతం (దోష సామ్యం - దూకుడు తటస్థీకరించబడింది)\n" : "  + 4 pts | Kuja Dosha    : EXCELLENT (Dosha Samya - Aggression neutralized)\n"); }
    else { s_kuja = 0; printf(te ? "  + 0 pts | కుజ దోషం       : అసమతుల్యం (వివాహంలో తీవ్రమైన వేడి మరియు ఘర్షణ)\n" : "  + 0 pts | Kuja Dosha    : ASYMMETRIC (Volatile marital heat)\n"); }

    bool nd1 = get_nodal_dosha(p1_ra, p1_asc);
    bool nd2 = get_nodal_dosha(p2_ra, p2_asc);
    if (nd1 == nd2) { s_nodal_dosha = 5; printf(te ? "  + 5 pts | రాహు/కేతు సామ్యం : సమతుల్యం (ఛాయా గ్రహ దోషాలు తటస్థీకరించబడ్డాయి)\n" : "  + 5 pts | Nodal Samya   : BALANCED (Shadow nodes neutralized)\n"); }
    else { s_nodal_dosha = 0; printf(te ? "  + 0 pts | రాహు/కేతు సామ్యం : అసమతుల్యం (ప్రమాదకరమైన నోడల్ అసమతుల్యత)\n" : "  + 0 pts | Nodal Samya   : ASYMMETRIC (Fatal nodal imbalance)\n"); }

    if (rajju_arr[nak_1] == rajju_arr[nak_2]) { 
        s_rajju = -10; printf(te ? " -10 pts | రజ్జు కూటమి     : ఏక రజ్జు దోషం (ప్రాణ గండం - ఆయుష్షుకు ముప్పు)\n" : " -10 pts | Rajju Kuta    : FATAL DOSHA (Same Rajju - Threat to longevity)\n"); 
    } else { s_rajju = 0; printf(te ? "  + 0 pts | రజ్జు కూటమి     : సురక్షితం (వేర్వేరు రజ్జువులు)\n" : "  + 0 pts | Rajju Kuta    : SAFE (Different Rajjus)\n"); }

    double comp_mo = get_midpoint(p1.planet_lons[2], p2.planet_lons[2]);
    double comp_ra = get_midpoint(p1.planet_lons[8], p2.planet_lons[8]);
    if (check_dist(comp_mo, p1.planet_lons[8], 10.0) || check_dist(comp_mo, p2.planet_lons[8], 10.0) || check_dist(comp_mo, comp_ra, 10.0)) { 
        s_comp = 4; printf(te ? "  + 4 pts | ఉమ్మడి గ్రహ స్థితి : విధి నిర్ణయం (కాంపోజిట్ చంద్రుడు రాహువుతో కలయిక)\n" : "  + 4 pts | Composite     : FATED BOND (Composite Moon conjunct Rahu)\n"); 
    } else { s_comp = 1; printf(te ? "  + 1 pts | ఉమ్మడి గ్రహ స్థితి : సాధారణం\n" : "  + 1 pts | Composite     : STANDARD (No profound composite planetary alignments)\n"); }

    int deb_signs[] = {-1, 6, 7, 3, 11, 9, 5, 0}; 
    if (p1_d30_7L != deb_signs[p1_7L] && p2_d30_7L != deb_signs[p2_7L]) {
        s_d30 = 2; printf(te ? "  + 2 pts | D30 త్రింశాంశ   : దోష రహితం (7వ అధిపతులు దాగి ఉన్న దోషాల నుండి విముక్తం)\n" : "  + 2 pts | D30 Trimsamsa : CLEAN (7th Lords free from deep hidden afflictions)\n");
    } else { s_d30 = 0; printf(te ? "  + 0 pts | D30 త్రింశాంశ   : బాధాకరం (దాగి ఉన్న వైవాహిక కర్మ / దోషాలు ఉన్నాయి)\n" : "  + 0 pts | D30 Trimsamsa : AFFLICTED (Hidden marital karma / evils present)\n"); }

    if (is_pushkara(p1.get_varga(9, p1.planet_lons[6])) || is_pushkara(p2.get_varga(9, p2.planet_lons[2]))) {
        s_pushkara = 2; printf(te ? "  + 2 pts | పుష్కర భాగ     : దైవిక ఆశీర్వాదం (నవాంశలో శుక్రుడు/చంద్రుడు పుష్కర భాగలో ఉన్నారు)\n" : "  + 2 pts | Pushkara Bhaga: DIVINE BLESSING (Venus/Moon in Pushkara Navamsa)\n");
    } else { s_pushkara = 0; printf(te ? "  + 0 pts | పుష్కర భాగ     : సాధారణం\n" : "  + 0 pts | Pushkara Bhaga: STANDARD (No Pushkara Navamsa protection)\n"); }

    total_score += (s_sat + s_7L + s_kuja + s_comp + s_d30 + s_pushkara + s_nodal_dosha + s_rajju);

    // =================================================================
    // 5. NAVAMSA (D9) & TIMING MANIFESTATION (15 Points Max)
    // =================================================================
    if (te) printf("\n[5. నవాంశ (D9) & సమయ అనుకూలత (15 pts)]\n");
    else printf("\n[5. NAVAMSA (D9) & TIMING MANIFESTATION (15 pts)]\n");
    
    int s_d9 = 0, s_time = 0;
    
    if (p1_d9_asc == p2_asc || p1_d9_asc == p2_mo || p2_d9_asc == p1_asc || p2_d9_asc == p1_mo || p1_d9_asc == p2_d9_asc) {
        s_d9 = 4; printf(te ? "  + 4 pts | D9 లగ్న అనుసంధానం : ఆత్మ గుర్తింపు (D9 లగ్నాలు D1 లగ్నాలు/చంద్రులతో కచ్చితంగా కలిశాయి)\n" : "  + 4 pts | D9 Lagna Lock : SOUL RECOGNITION (D9 Lagnas exactly overlay D1 Lagnas/Moons)\n");
    } else if (get_house(p1_d9_asc, p2_d9_asc)==1 || get_house(p1_d9_asc, p2_d9_asc)==5 || get_house(p1_d9_asc, p2_d9_asc)==9 || get_house(p1_d9_asc, p2_d9_asc)==7) {
        s_d9 = 2; printf(te ? "  + 2 pts | D9 లగ్న అక్షం   : అనుకూలం (D9 లగ్నాలు 1/5/9 లేదా 1/7 సంబంధంలో ఉన్నాయి)\n" : "  + 2 pts | D9 Lagna Axis : ALIGNED (D9 Lagnas in 1/5/9 or 1/7 relationship)\n");
    } else { s_d9 = 0; printf(te ? "  + 0 pts | D9 లగ్న అక్షం   : స్వతంత్రం (నవాంశలో వేర్వేరు ఆత్మ మార్గాలు)\n" : "  + 0 pts | D9 Lagna Axis : DIVERGENT (Independent soul paths in Navamsa)\n"); }

    int d9_ve_dist = get_house(p1_d9_ve, p2_d9_ve);
    if (d9_ve_dist == 7) {
        s_d9 += 3; printf(te ? "  + 3 pts | D9 శుక్ర బంధం   : ఆత్మల ఆకర్షణ (D9 శుక్రులు పరస్పర దృష్టిలో ఉన్నారు - అత్యంత అయస్కాంత ఆకర్షణ)\n" : "  + 3 pts | D9 Venus Axis : SOUL POLARITY (D9 Venuses are opposite - highly magnetic)\n");
    }

    time_t t = time(nullptr); tm* now = gmtime(&t);
    double ut_dec = now->tm_hour + (now->tm_min / 60.0) + (now->tm_sec / 3600.0);
    double current_jd = swe_julday(now->tm_year + 1900, now->tm_mon + 1, now->tm_mday, ut_dec, SE_GREG_CAL);

    int p1_md, p1_ad, p2_md, p2_ad;
    p1.get_active_dasha_lords(current_jd, p1_md, p1_ad);
    p2.get_active_dasha_lords(current_jd, p2_md, p2_ad);

    int d_map[] = {-1, 2, 3, 4, 8, 6, 1, 7, 5, 0}; 
    int dasha_7L_1 = d_map[p1_7L], dasha_7L_2 = d_map[p2_7L];
    auto is_activator = [&](int md, int ad, int d7) { return (md==1||md==6||md==5||md==d7 || ad==1||ad==6||ad==5||ad==d7); };
    
    bool p1_active = is_activator(p1_md, p1_ad, dasha_7L_1);
    bool p2_active = is_activator(p2_md, p2_ad, dasha_7L_2);

    if (te) {
        printf("  - వ్యక్తి 1 దశ : %s / %s -> వివాహ సమయ అనుకూలత: %s\n", te_dasha_lords[p1_md], te_dasha_lords[p1_ad], p1_active ? "అధికం" : "తక్కువ");
        printf("  - వ్యక్తి 2 దశ : %s / %s -> వివాహ సమయ అనుకూలత: %s\n", te_dasha_lords[p2_md], te_dasha_lords[p2_ad], p2_active ? "అధికం" : "తక్కువ");
    } else {
        printf("  - Person 1 Dasha : %s / %s -> Readiness: %s\n", dasha_lords[p1_md], dasha_lords[p1_ad], p1_active ? "HIGH" : "Low");
        printf("  - Person 2 Dasha : %s / %s -> Readiness: %s\n", dasha_lords[p2_md], dasha_lords[p2_ad], p2_active ? "HIGH" : "Low");
    }

    if (p1_active && p2_active) { s_time = 8; printf(te ? "  + 8 pts | దశా అనుకూలత   : సమకాలీకరించబడింది (విశ్వం మీ కలయికను చురుకుగా ప్రోత్సహిస్తోంది)\n" : "  + 8 pts | Manifestation : SYNCHRONIZED (Universe is actively pushing union)\n"); }
    else if (p1_active || p2_active) { s_time = 4; printf(te ? "  + 4 pts | దశా అనుకూలత   : అసమతుల్యత (సమయం సరిగ్గా లేదు; ఒక భాగస్వామికి జాప్యం ఉంది)\n" : "  + 4 pts | Manifestation : ASYMMETRIC (Timing is off; one partner is delayed)\n"); }
    else { s_time = 0; printf(te ? "  + 0 pts | దశా అనుకూలత   : నిద్రాణస్థితి (ప్రస్తుతం వివాహ సమయం యాక్టివ్‌గా లేదు)\n" : "  + 0 pts | Manifestation : DORMANT (No marital timing activated currently)\n"); }

    total_score += (s_d9 + s_time);

    // =================================================================
    // 6. MUTUAL RECTIFICATION & DESTRUCTION (15 Points Max)
    // =================================================================
    if (te) printf("\n[6. దోష పరిహారాలు & నష్టాలు - పరస్పర శాంతి (15 pts)]\n");
    else printf("\n[6. MUTUAL RECTIFICATION & DESTRUCTION - Dosha Nullification (15 pts)]\n");
    
    int s_rect = 0;

    auto check_afflictions = [&](int mo, int ve, int l7, int sa, int ra, int ke) {
        int count = 0;
        if (mo==sa || mo==ra || mo==ke) count++;
        if (ve==sa || ve==ra || ve==ke) count++;
        if (l7==sa || l7==ra || l7==ke) count++;
        return count;
    };
    
    int p1_afflictions = check_afflictions(p1_mo, p1_ve, p1.planet_rashis[p1_7L], p1_sa, p1_ra, p1_ke);
    int p2_afflictions = check_afflictions(p2_mo, p2_ve, p2.planet_rashis[p2_7L], p2_sa, p2_ra, p2_ke);

    auto check_healing = [&](int mo, int ve, int l7, int partner_ju) {
        int heals = 0;
        int d_mo = get_house(mo, partner_ju); if(d_mo==1||d_mo==5||d_mo==7||d_mo==9) heals++;
        int d_ve = get_house(ve, partner_ju); if(d_ve==1||d_ve==5||d_ve==7||d_ve==9) heals++;
        int d_l7 = get_house(l7, partner_ju); if(d_l7==1||d_l7==5||d_l7==7||d_l7==9) heals++;
        return heals;
    };

    auto check_destruction = [&](int mo, int ve, int l7, int partner_sa, int partner_ra, int partner_ke) {
        int damages = 0;
        if (mo==partner_sa || mo==partner_ra || mo==partner_ke) damages++;
        if (ve==partner_sa || ve==partner_ra || ve==partner_ke) damages++;
        if (l7==partner_sa || l7==partner_ra || l7==partner_ke) damages++;
        return damages;
    };

    int p2_heals_p1 = check_healing(p1_mo, p1_ve, p1.planet_rashis[p1_7L], p2_ju);
    int p1_heals_p2 = check_healing(p2_mo, p2_ve, p2.planet_rashis[p2_7L], p1_ju);
    
    int p2_destroys_p1 = check_destruction(p1_mo, p1_ve, p1.planet_rashis[p1_7L], p2_sa, p2_ra, p2_ke);
    int p1_destroys_p2 = check_destruction(p2_mo, p2_ve, p2.planet_rashis[p2_7L], p1_sa, p1_ra, p1_ke);

    double p1_rect_pct = (p2_afflictions > 0) ? std::min(100.0, ((double)p1_heals_p2 / p2_afflictions) * 100.0) : 100.0;
    double p2_rect_pct = (p1_afflictions > 0) ? std::min(100.0, ((double)p2_heals_p1 / p1_afflictions) * 100.0) : 100.0;

    if (te) {
        printf("  - వ్యక్తి 1 గ్రహ దోషాలు : %d | వ్యక్తి 2 ద్వారా పరిహరించబడినవి: %.0f%%\n", p1_afflictions, p2_rect_pct);
        printf("  - వ్యక్తి 2 గ్రహ దోషాలు : %d | వ్యక్తి 1 ద్వారా పరిహరించబడినవి: %.0f%%\n", p2_afflictions, p1_rect_pct);
    } else {
        printf("  - Person 1 Internal Afflictions : %d | Rectified by P2: %.0f%%\n", p1_afflictions, p2_rect_pct);
        printf("  - Person 2 Internal Afflictions : %d | Rectified by P1: %.0f%%\n", p2_afflictions, p1_rect_pct);
    }

    int net_healing = (p1_heals_p2 * 3) + (p2_heals_p1 * 3);
    int net_damage = (p1_destroys_p2 * 4) + (p2_destroys_p1 * 4);
    
    s_rect = 5 + net_healing - net_damage; // Base score 5
    if (s_rect > 15) s_rect = 15;
    if (s_rect < 0) s_rect = 0;

    if (net_damage > net_healing) {
        printf(te ? "  + %d pts | పరస్పర దోష పరిహారం : దోష తీవ్రత పెరుగుదల (ఒకరి దోషాలను మరొకరు పెంచుతున్నారు)\n" : "  + %d pts | Dosha Exchange  : AGGRAVATION (Charts compound each other's malefic flaws)\n", s_rect);
    } else if (net_healing > 0) {
        printf(te ? "  + %d pts | పరస్పర దోష పరిహారం : పరిహారం (ఒకరి అంతర్గత దోషాలను మరొకరు విజయవంతంగా తగ్గిస్తున్నారు)\n" : "  + %d pts | Dosha Exchange  : HEALING (Charts successfully nullify internal afflictions)\n", s_rect);
    } else {
        printf(te ? "  + %d pts | పరస్పర దోష పరిహారం : సాధారణం (పెద్దగా ఒకరికొకరు నష్టం/మేలు చేయడం లేదు)\n" : "  + %d pts | Dosha Exchange  : NEUTRAL (No major cross-chart healing or destruction)\n", s_rect);
    }

    total_score += s_rect;

    // =================================================================
    // FINAL VERDICT NORMALIZATION
    // =================================================================
    int max_score = 165; // 29 + 33 + 42 + 31 + 15 + 15 = 165 Maximum Points
    int final_percentage = (int)round(((double)total_score / max_score) * 100.0);
    
    printf("=================================================================\n");
    if (te) printf("తుది V8.4 సార్వత్రిక జాతక పొంతన స్కోరు: %d / %d పాయింట్లు (శాతం: %d%%)\n", (int)total_score, max_score, final_percentage);
    else printf("FINAL V8.4 UNIVERSAL SYNASTRY INDEX: %d / %d Raw Points (Normalized: %d%%)\n", (int)total_score, max_score, final_percentage);
    
    if (final_percentage >= 80) {
        printf(te ? "ఫలితం: అరుదైన ఆత్మ బంధం (ప్రగాఢమైన కర్మ, శారీరక మరియు ఆధ్యాత్మిక కలయిక. దశా సమయం అనుకూలంగా ఉంది.)\n" : "STATUS: RARE SOULMATE (Profound karmic, physical, and spiritual union. Timing is aligned.)\n");
    } else if (final_percentage >= 60) {
        printf(te ? "ఫలితం: అత్యంత శుభకరం (బలమైన ప్రేమ, నిర్మాణపరమైన దీర్ఘాయువు మరియు అద్భుతమైన అనుకూలత.)\n" : "STATUS: HIGHLY AUSPICIOUS (Strong love, structural longevity, and excellent compatibility.)\n");
    } else if (final_percentage >= 40) {
        printf(te ? "ఫలితం: సాధారణం (సాధారణ మానవ సంబంధం; రాజీ మరియు ఓర్పు అవసరం.)\n" : "STATUS: AVERAGE (Standard human connection; requires compromise and patience.)\n");
    } else {
        printf(te ? "ఫలితం: తీవ్రమైన ఘర్షణ / కర్మ రుణం (శాంతి కోసం సిఫార్సు చేయబడదు; నిర్మాణపరమైన అడ్డంకులు ఉన్నాయి.)\n" : "STATUS: HIGH FRICTION / KARMIC DEBT (Not recommended for peace; structural obstacles detected.)\n");
    }
    printf("=================================================================\n");
}

void predict_synastry_events(const JyotishaEngine& p1, const JyotishaEngine& p2, int start_year, int end_year) {
    printf("\n=================================================================\n");
    printf("=== MUTUAL EVENT SCANNER (SYNASTRY DOUBLE-BLIND CROSS-TRANSIT) ===\n");
    printf("=================================================================\n");
    printf("Scanning %d to %d for days where BOTH charts hit critical mass...\n\n", start_year, end_year);

    auto calculate_person_score = [](const JyotishaEngine& p, double jd, string& reason, string& env, string& archetype) -> int {
        int score = 0;
        int asc_rashi = p.planet_rashis[0];
        
        // --- Core Lords ---
        int h7_rashi = (asc_rashi + 6) % 12;
        int l7_idx = 1; for(int x=1; x<=7; x++) if(string(rashi_lords[h7_rashi]) == p_names_full[x]) l7_idx = x;
        
        int h2_rashi = (asc_rashi + 1) % 12; 
        int l2_idx = 1; for(int x=1; x<=7; x++) if(string(rashi_lords[h2_rashi]) == p_names_full[x]) l2_idx = x;
        
        int h9_rashi = (asc_rashi + 8) % 12; 
        int l9_idx = 1; for(int x=1; x<=7; x++) if(string(rashi_lords[h9_rashi]) == p_names_full[x]) l9_idx = x;

        int ul_rashi = p.get_arudha(12);
        int dk_idx = p.darakaraka_idx;
        int dk_rashi = p.planet_rashis[dk_idx];
        
        int d9_asc = p.get_varga(9, p.planet_lons[0]);
        int d9_h7 = (d9_asc + 6) % 12;
        int d9_7L_idx = 1;
        for(int x=1; x<=7; x++) if(string(rashi_lords[d9_h7]) == p_names_full[x]) d9_7L_idx = x;
        int d9_7L_rashi = p.get_varga(9, p.planet_lons[d9_7L_idx]);

        auto get_trigger_name = [&](int r) {
            string res = "";
            if (r == h7_rashi) res += "H7/";
            if (r == ul_rashi) res += "UL/";
            if (r == asc_rashi) res += "Asc/";
            if (r == p.planet_rashis[l7_idx]) res += "7L/";
            if (r == dk_rashi) res += "DK/";
            if (res.length() > 0) res.pop_back();
            return res;
        };

        // --- Dasha Readiness ---
        int d_map[] = {9, 6, 1, 2, 3, 8, 5, 7, 4}; 
        int md, ad; p.get_active_dasha_lords(jd, md, ad);
        int md_p = d_map[md]; int ad_p = d_map[ad];
        
        bool md_active = (md_p == l7_idx || md_p == l2_idx || md_p == l9_idx || md_p == 6 || md_p == 8 || md_p == 5 || p.planet_rashis[md_p] == ul_rashi || md_p == dk_idx);
        bool ad_active = (ad_p == l7_idx || ad_p == l2_idx || ad_p == l9_idx || ad_p == 6 || ad_p == 8 || ad_p == 5 || p.planet_rashis[ad_p] == ul_rashi || ad_p == dk_idx);
        
        if (md_active) score += 3;
        if (ad_active) score += 4;

        int ad_house = (p.planet_rashis[ad_p] - asc_rashi + 12) % 12 + 1;
        if (ad_house == 3 || ad_house == 4) env = "Local/Internet";
        else if (ad_house == 9) env = "Travel/Education";
        else if (ad_house == 10 || ad_house == 11) env = "Work/Network";
        else if (ad_house == 2 || ad_house == 7) env = "Direct/Family";
        else env = "Social setting";

        // --- Transit Checks ---
        double xx_ju[6], xx_sa[6], xx_ve[6], xx_su[6]; char serr[256];
        swe_calc_ut(jd, SE_JUPITER, p.iflag, xx_ju, serr);
        int t_ju_rashi = (int)(xx_ju[0] / 30.0);
        auto ju_aspects = [&](int r) { int d = (r - t_ju_rashi + 12) % 12 + 1; return (d==1||d==5||d==7||d==9); };
        
        bool ju_hit_d1 = (ju_aspects(h7_rashi) || ju_aspects(p.planet_rashis[l7_idx]) || ju_aspects(ul_rashi) || ju_aspects(asc_rashi) || ju_aspects(dk_rashi));
        bool ju_hit_d9 = (ju_aspects(d9_asc) || ju_aspects(d9_7L_rashi));
        if (ju_hit_d1) score += 4;
        if (ju_hit_d9) { score += 2; reason += "[D9 Jup] "; }

        swe_calc_ut(jd, SE_SATURN, p.iflag, xx_sa, serr);
        int t_sa_rashi = (int)(xx_sa[0] / 30.0);
        auto sa_aspects = [&](int r) { int d = (r - t_sa_rashi + 12) % 12 + 1; return (d==1||d==3||d==7||d==10); };
        
        bool sa_hit = (sa_aspects(h7_rashi) || sa_aspects(p.planet_rashis[l7_idx]) || sa_aspects(ul_rashi) || sa_aspects(asc_rashi) || sa_aspects(dk_rashi));
        if (sa_hit) score += 4;

        if (ju_hit_d1 && sa_hit && (md_active || ad_active)) {
            swe_calc_ut(jd, SE_VENUS, p.iflag, xx_ve, serr);
            int t_ve_rashi = (int)(xx_ve[0]/30.0);
            if (t_ve_rashi == h7_rashi || t_ve_rashi == ul_rashi || t_ve_rashi == asc_rashi || t_ve_rashi == p.planet_rashis[l7_idx] || t_ve_rashi == dk_rashi) {
                score += 3; reason += "[Ven in " + string(short_rashi[t_ve_rashi]) + " hits " + get_trigger_name(t_ve_rashi) + "] ";
            }
            swe_calc_ut(jd, SE_SUN, p.iflag, xx_su, serr);
            int t_su_rashi = (int)(xx_su[0]/30.0);
            if (t_su_rashi == h7_rashi || t_su_rashi == ul_rashi || t_su_rashi == asc_rashi || t_su_rashi == p.planet_rashis[l7_idx] || t_su_rashi == dk_rashi) {
                score += 2; reason += "[Sun in " + string(short_rashi[t_su_rashi]) + " hits " + get_trigger_name(t_su_rashi) + "] ";
                
                // --- LIFE PATTERN ARCHETYPE DEDUCTION ---
                int hit_house = (t_su_rashi - asc_rashi + 12) % 12 + 1;
                if (hit_house == 5 || hit_house == 9) archetype = "Creation/Lineage";
                else if (hit_house == 1 || hit_house == 7) archetype = "Union/Partnership";
                else if (hit_house == 2 || hit_house == 11) archetype = "Family Expand/Gains";
                else if (hit_house == 4 || hit_house == 12) archetype = "Foundation/Hospital";
                else if (hit_house == 10) archetype = "Status Shift/Career";
                else archetype = "Social/Karmic Shift";
            }
            
            if (archetype == "" && t_ve_rashi >= 0) {
                int hit_house = (t_ve_rashi - asc_rashi + 12) % 12 + 1;
                if (hit_house == 5 || hit_house == 9) archetype = "Creation/Lineage";
                else if (hit_house == 1 || hit_house == 7) archetype = "Union/Partnership";
                else if (hit_house == 2 || hit_house == 11) archetype = "Family Expand/Gains";
                else if (hit_house == 4 || hit_house == 12) archetype = "Foundation/Hospital";
                else archetype = "Social/Karmic Shift";
            }

        } else { score = 0; } 
        
        char md_buf[32]; snprintf(md_buf, sizeof(md_buf), "[%s/%s] ", dasha_lords[md], dasha_lords[ad]);
        reason = string(md_buf) + reason;
        return score;
    };

    double start_jd = swe_julday(start_year, 1, 1, 0.0, SE_GREG_CAL);
    double end_jd = swe_julday(end_year, 12, 31, 0.0, SE_GREG_CAL);

    // V9.1 Addition: Added t_pos to store the Cosmic Metronome degrees
    struct Hit { int y, m, d; int s1, s2, total; string cons_env, arch1, arch2, res1, res2, t_pos; };
    vector<Hit> peak_hits;

    for (double jd = start_jd; jd <= end_jd; jd += 1.0) {
        string r1 = "", e1 = "", a1 = "", r2 = "", e2 = "", a2 = "";
        int s1 = calculate_person_score(p1, jd, r1, e1, a1);
        int s2 = calculate_person_score(p2, jd, r2, e2, a2);

        if (s1 >= 11 && s2 >= 11 && (s1 >= 16 || s2 >= 16)) {
            int y, m, d; double jut;
            swe_revjul(jd + (p1.location.tz_offset/24.0), SE_GREG_CAL, &y, &m, &d, &jut);
            
            string combined_env = (e1 == e2) ? e1 : e1 + " & " + e2;

            // Extract the exact degrees of the Sun and Venus to map the pattern
            double xx_su[6], xx_ve[6]; char serr[256];
            swe_calc_ut(jd, SE_SUN, p1.iflag, xx_su, serr);
            swe_calc_ut(jd, SE_VENUS, p1.iflag, xx_ve, serr);
            char pos_buf[64];
            snprintf(pos_buf, sizeof(pos_buf), "Su:%02d°%s Ve:%02d°%s", 
                     (int)fmod(xx_su[0], 30.0), short_rashi[(int)(xx_su[0]/30.0)],
                     (int)fmod(xx_ve[0], 30.0), short_rashi[(int)(xx_ve[0]/30.0)]);

            bool added = false;
            for (auto& hit : peak_hits) {
                if (hit.y == y && hit.m == m) {
                    if ((s1 + s2) > hit.total) { 
                        hit.d = d; hit.s1 = s1; hit.s2 = s2; hit.total = s1+s2; 
                        hit.cons_env = combined_env; hit.res1 = r1; hit.res2 = r2; 
                        hit.arch1 = a1; hit.arch2 = a2; hit.t_pos = string(pos_buf); 
                    }
                    added = true; break;
                }
            }
            if (!added) peak_hits.push_back({y, m, d, s1, s2, s1+s2, combined_env, a1, a2, r1, r2, string(pos_buf)});
        }
    }

    if (peak_hits.empty()) {
        printf("No mutual convergence found. The cosmic clocks for these two charts do not align in this timeframe.\n");
    } else {
        sort(peak_hits.begin(), peak_hits.end(), [](const Hit& a, const Hit& b) { 
            if (a.y != b.y) return a.y < b.y;
            if (a.m != b.m) return a.m < b.m;
            return a.d < b.d;
        });

        // V9.1 Upgrade: New "Cosmic Metronome" column added to the print layout
        printf("%-10s | %-12s | %-12s | %-19s | %-30s | %-20s | %-20s\n", 
               "Mutual Day", "P1 Power", "P2 Power", "Cosmic Clock", "Consolidated Env", "P1 Life Theme", "P2 Life Theme");
        printf("----------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
        for (const auto& hit : peak_hits) {
            printf("%02d/%02d/%04d | P1: %d/22 pts | P2: %d/22 pts | %-19s | %-30s | %-20s | %-20s\n", 
                   hit.d, hit.m, hit.y, hit.s1, hit.s2, hit.t_pos.c_str(), hit.cons_env.c_str(), hit.arch1.c_str(), hit.arch2.c_str());
            printf("           | %-120s \n", hit.res1.c_str());
            printf("           | %-120s \n", hit.res2.c_str());
            printf("----------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
        }
    }
    printf("=================================================================\n");
}

void run_ayush_analysis(JyotishaEngine& p) {
    // Silently calculate necessary data if running standalone
	p.analyze_auspiciousness(p.planet_rashis[0], p.planet_rashis);
    if (!p.av_calculated) p.calculate_ashtakavarga(true);
    
    printf("\n=================================================================================================\n");
    printf("=== ADVANCED AYUSH (LONGEVITY & VITALITY) ANALYSIS MODULE ===\n");
    printf("=================================================================================================\n");
    printf("DISCLAIMER: Classical Ayurdaya calculations evaluate physical vitality and sensitive health windows,\n");
    printf("not exact fatalistic predictions. Modern medicine heavily mitigates these classical algorithms.\n");
    printf("=================================================================================================\n\n");

    int asc_rashi = p.planet_rashis[0];
    
    // --- 1. AYUSH TRIKONA (The Longevity Triangle) ---
    int h1_rashi = asc_rashi;
    int h3_rashi = (asc_rashi + 2) % 12;
    int h8_rashi = (asc_rashi + 7) % 12;
    
    auto get_lord_idx = [&](int rashi) {
        for(int x=1; x<=7; x++) if(string(rashi_lords[rashi]) == p_names_full[x]) return x;
        return 1;
    };

    int l1_idx = get_lord_idx(h1_rashi);
    int l3_idx = get_lord_idx(h3_rashi);
    int l8_idx = get_lord_idx(h8_rashi);
    
    printf("[STEP 1: THE AYUSH TRIKONA (PILLARS OF VITALITY)]\n");
    printf("%-15s | %-10s | %-10s | %-45s\n", "House", "Sign", "Lord", "Vitality Status");
    printf("-------------------------------------------------------------------------------------------------\n");
    
    auto print_trikona_status = [&](string name, int rashi, int l_idx) {
        string status = (p.natal_scores[l_idx] >= 5) ? "Strong (High Life Force)" : 
                        (p.natal_scores[l_idx] >= 0) ? "Stable (Average Vitality)" : "Afflicted (Vulnerable)";
        printf("%-15s | %-10s | %-10s | %-45s\n", name.c_str(), rashi_names[rashi], p_names_full[l_idx], status.c_str());
    };
    
    print_trikona_status("1st (Lagna)", h1_rashi, l1_idx);
    print_trikona_status("3rd (Prana)", h3_rashi, l3_idx);
    print_trikona_status("8th (Ayush)", h8_rashi, l8_idx);
    printf("-------------------------------------------------------------------------------------------------\n\n");

    // --- 2. AYUSH KARAKAS (Significators) ---
    printf("[STEP 2: NATURAL LONGEVITY KARAKAS]\n");
    int sa_score = p.natal_scores[7]; 
    int ju_score = p.natal_scores[5]; 
    
    printf("Saturn (Ayushkaraka) : %s [Score: %d] - Dictates physical endurance and structural longevity.\n", 
           (sa_score >= 0) ? "SUPPORTIVE" : "VULNERABLE", sa_score);
    printf("Jupiter (Jeevakaraka): %s [Score: %d] - Dictates divine protection and recovery from illness.\n", 
           (ju_score >= 0) ? "SUPPORTIVE" : "VULNERABLE", ju_score);
           
    double ju_lon = p.planet_lons[5];
    int ju_rashi = (int)(ju_lon / 30.0);
    int d5 = (ju_rashi + 4) % 12; int d7 = (ju_rashi + 6) % 12; int d9 = (ju_rashi + 8) % 12;
    if (d5 == h8_rashi || d7 == h8_rashi || d9 == h8_rashi) {
        printf(">>> DIVINE SHIELD DETECTED: Jupiter casts a direct protective aspect onto the 8th House!\n");
    }
    printf("\n");

    // --- 3. MATHEMATICAL TRIAYURDAYA (Lifespan Calculations) ---
    printf("[STEP 3: TRIAYURDAYA ESTIMATIONS]\n");
    
    double pindayu_max[8] = {0, 19.0, 25.0, 15.0, 12.0, 15.0, 21.0, 20.0};
    double nisargayu_base[8] = {0, 20.0, 1.0, 2.0, 9.0, 18.0, 20.0, 50.0};

    double total_pindayu = 0;
    double total_nisargayu = 0;
    double total_amsayu = 0;

    for (int i = 1; i <= 7; i++) {
        double deg_in_sign = fmod(p.planet_lons[i], 30.0);
        double fraction = deg_in_sign / 30.0;
        
        double p_years = fraction * pindayu_max[i];
        
        int navamsa_completed = (int)(deg_in_sign / 3.333333);
        double a_years = (navamsa_completed + 1) * 1.5; 
        
        double strength_ratio = std::max(0.5, 1.0 + (p.natal_scores[i] / 20.0)); 
        double n_years = nisargayu_base[i] * strength_ratio;

        if (p.natal_scores[i] <= -5) {
            p_years *= 0.5; a_years *= 0.5; 
        }
        else if (p.natal_scores[i] >= 5) {
            p_years *= 1.5; a_years *= 1.5; 
        }

        total_pindayu += p_years;
        total_amsayu += a_years;
        total_nisargayu += n_years;
    }

    double asc_fraction = fmod(p.planet_lons[0], 30.0) / 30.0;
    total_pindayu += (asc_fraction * 19.0);

    total_pindayu = std::min(total_pindayu, 120.0);
    total_amsayu = std::min(total_amsayu, 120.0);
    total_nisargayu = std::min(total_nisargayu, 120.0);

    printf("%-20s : %.2f Years (Based on Degrees Traversed)\n", "Pindayu Method", total_pindayu);
    printf("%-20s : %.2f Years (Based on D9 Navamsa Positions)\n", "Amsayu Method", total_amsayu);
    printf("%-20s : %.2f Years (Based on Natural/Shadbala Strength)\n", "Nisargayu Method", total_nisargayu);
    printf("-------------------------------------------------------------------------------------------------\n\n");

    // --- 4. ASHTAKAVARGA AYURDAYA (Video Method) ---
    printf("[STEP 4: ASHTAKAVARGA AYURDAYA (SHODHYA PINDA METHOD)]\n");
    printf("-> Evaluated using exact Trikona and Ekadhipatya Shodhana mathematical reductions.\n\n");

    int rasi_gunakara[12] = {7, 10, 8, 4, 10, 5, 7, 8, 9, 5, 11, 12};
    int graha_gunakara[7] = {5, 5, 8, 5, 10, 7, 5}; 
    double total_ashtakavarga_ayush = 0;

    for (int graha = 0; graha < 7; graha++) {
        int bav[12];
        for(int r=0; r<12; r++) bav[r] = p.bav_scores[graha][r];

        // 1. Trikona Shodhana (Trinal Reduction)
        for (int trine = 0; trine < 4; trine++) {
            int t1 = trine, t2 = trine + 4, t3 = trine + 8;
            int v1 = bav[t1], v2 = bav[t2], v3 = bav[t3];
            
            if (v1 == 0 && v2 == 0 && v3 > 0) bav[t3] = 0;
            else if (v1 == 0 && v3 == 0 && v2 > 0) bav[t2] = 0;
            else if (v2 == 0 && v3 == 0 && v1 > 0) bav[t1] = 0;
            else if (v1 > 0 && v2 > 0 && v3 > 0) {
                int min_v = std::min(v1, std::min(v2, v3));
                bav[t1] -= min_v; bav[t2] -= min_v; bav[t3] -= min_v;
            }
        }

        // 2. Ekadhipatya Shodhana (Dual Lordship Reduction)
        int pairs[5][2] = {{0,7}, {1,6}, {2,5}, {8,11}, {9,10}};
        for (int i = 0; i < 5; i++) {
            int s1 = pairs[i][0];
            int s2 = pairs[i][1];
            bool occ1 = false, occ2 = false;
            for(int np=1; np<=7; np++) {
                if (p.planet_rashis[np] == s1) occ1 = true;
                if (p.planet_rashis[np] == s2) occ2 = true;
            }

            if (!occ1 && !occ2) {
                if (bav[s1] == bav[s2]) { bav[s1] = 0; bav[s2] = 0; }
                else { int min_v = std::min(bav[s1], bav[s2]); bav[s1] = min_v; bav[s2] = min_v; }
            } else if (occ1 && !occ2) {
                if (bav[s1] >= bav[s2]) bav[s2] = 0;
                else bav[s2] = bav[s1];
            } else if (!occ1 && occ2) {
                if (bav[s2] >= bav[s1]) bav[s1] = 0;
                else bav[s1] = bav[s2];
            }
        }

        // 3. Calculate Shodhya Pinda (Sum of Multipliers)
        int shodhya_pinda = 0;
        for (int r = 0; r < 12; r++) {
            if (bav[r] > 0) {
                shodhya_pinda += (bav[r] * rasi_gunakara[r]);
                for (int np = 1; np <= 7; np++) {
                    if (p.planet_rashis[np] == r) {
                        shodhya_pinda += (bav[r] * graha_gunakara[np-1]);
                    }
                }
            }
        }

        // 4. Calculate Final Years
        double ayush_years = (shodhya_pinda * 7.0) / 27.0;
        total_ashtakavarga_ayush += ayush_years;
    }

    total_ashtakavarga_ayush = std::min(total_ashtakavarga_ayush, 120.0);
    
    printf("%-20s : %.2f Years (Highly Accurate Post-Reduction Method)\n", "Ashtakavarga Ayush", total_ashtakavarga_ayush);
    printf("-------------------------------------------------------------------------------------------------\n");
    
    double average_ayush = (total_pindayu + total_amsayu + total_nisargayu + total_ashtakavarga_ayush) / 4.0;
    printf("FINAL AGGREGATED AVERAGE : %.2f Years\n", average_ayush);
    printf("-------------------------------------------------------------------------------------------------\n\n");

    // --- 5. MARAKA & VULNERABILITY ANALYSIS ---
    printf("[STEP 5: MARAKA (DEATH-INFLICTING) PLANETS]\n");
    int h2_rashi = (asc_rashi + 1) % 12;
    int h7_rashi = (asc_rashi + 6) % 12;
    int l2_idx = get_lord_idx(h2_rashi);
    int l7_idx = get_lord_idx(h7_rashi);

    printf("Primary Maraka Lord (2nd House): %s\n", p_names_full[l2_idx]);
    printf("Primary Maraka Lord (7th House): %s\n", p_names_full[l7_idx]);
    printf("-> Rule: The Mahadasha or Antardasha of these planets during old age or severe illness marks\n");
    printf("   the highest probability of physical transition. Extreme caution required if Saturn transits here.\n\n");

    // --- 6. FINAL CLASSIFICATION ---
    printf("[FINAL VITALITY CLASSIFICATION]\n");
    string category = "";
    if (average_ayush <= 36.0) category = "ALPAYU (Short Lifespan Tendency - Heavy Medical Mitigation Required)";
    else if (average_ayush <= 72.0) category = "MADHYAYU (Medium Lifespan Tendency - Standard Vulnerabilities)";
    else category = "PURNAYU (Long Lifespan Tendency - Strong Cosmic Protection)";

    printf(">>> CATEGORY: %s\n", category.c_str());
    printf("=================================================================================================\n");
}

// =========================================================================
// MAIN COMMAND LINE PARSER
// =========================================================================

int main(int argc, char *argv[]) {
    // --- TELUGU INTERCEPTOR ---
    bool telugu_ui = false;
    vector<char*> clean_args;
    clean_args.push_back(argv[0]);
    
    for (int i = 1; i < argc; i++) {
        if (strcasecmp(argv[i], "telugu") == 0 || strcasecmp(argv[i], "--te") == 0) {
            telugu_ui = true;
        } else {
            clean_args.push_back(argv[i]);
        }
    }
    
    int clean_argc = clean_args.size();
    char** clean_argv = clean_args.data();

    // 1. Help Menu Catch
    if (clean_argc < 8) { print_help_menu(); return 1; } 

    // 2. Parse Base Chart Requirements (These are mandatory for all commands)
    int year = stoi(clean_argv[1]), month = stoi(clean_argv[2]), day = stoi(clean_argv[3]);
    int hour = stoi(clean_argv[4]), minute = stoi(clean_argv[5]), second = stoi(clean_argv[6]);
    string target_city = clean_argv[7];

    auto it = find_if(city_db.begin(), city_db.end(), [&](const City& c) { return strcasecmp(c.name.c_str(), target_city.c_str()) == 0; });
    if (it == city_db.end()) { printf("Error: City '%s' not found.\n", target_city.c_str()); return 1; }

    // 3. Pre-Parse JSON Mode Flag (Needs to be passed to engine)
    bool json_mode = false;
    if (clean_argc >= 9 && strcasecmp(clean_argv[8], "json") == 0) json_mode = true;

    // 4. Initialize Core Engine (Global Access for all commands)
    JyotishaEngine engine(year, month, day, hour, minute, second, *it, json_mode, telugu_ui);
    
    // We run calculate_chart() here because 99% of commands rely on the Natal arrays being populated!
    engine.calculate_chart(); 

    // ========================================================
    // 5. COMMAND ROUTING
    // ========================================================
    if (clean_argc >= 9) {
        string cmd = clean_argv[8];
        
        // Command variables
        int t_year = 0, t_month = 0, t_day = 0;
        int t_hour = 12, t_min = 0, t_sec = 0; 
        bool time_provided = false;

        auto parse_target_time = [&](int start_idx) {
            if (clean_argc >= start_idx + 1) {
                string t_str = clean_argv[start_idx];
                if (t_str.find(':') != string::npos) {
                    sscanf(t_str.c_str(), "%d:%d:%d", &t_hour, &t_min, &t_sec); time_provided = true;
                } else if (clean_argc >= start_idx + 3) {
                    t_hour = stoi(clean_argv[start_idx]); t_min = stoi(clean_argv[start_idx+1]); t_sec = stoi(clean_argv[start_idx+2]); time_provided = true;
                }
            }
        };

        if (strcasecmp(cmd.c_str(), "json") == 0) {
            engine.calculate_ashtakavarga(true);
            engine.analyze_auspiciousness(engine.planet_rashis[0], engine.planet_rashis);
            engine.export_web_json(year, month, day); // Dummy target dates for JSON output
            return 0;
        }
        else if (strcasecmp(cmd.c_str(), "web_natal") == 0) return 0; 
        else if (strcasecmp(cmd.c_str(), "web_general") == 0) {
            int v_lord = -1, m_lord = -1;
            engine.calculate_varsha_masa(v_lord, m_lord);
            ShadbalaEngine::calculate(engine.lagna_lon, engine.planet_lons, engine.moon_lon, engine.tjd_ut, 
                                      engine.local_hour_decimal, engine.sunrise_hour_decimal, engine.sunset_hour_decimal, 
                                      engine.current_weekday, v_lord, m_lord, false, engine.json_output);
            engine.calculate_ashtakavarga(true); 
            engine.analyze_chart("D1"); 
            return 0;
        }
        else if (strcasecmp(cmd.c_str(), "web_dasha") == 0) {
			engine.calculate_ashtakavarga(true); 
            engine.analyze_auspiciousness(engine.planet_rashis[0], engine.planet_rashis);
			
            engine.calculate_dasha_balance();
            engine.print_dasha_web(); 
            return 0;
        }
        else if (strcasecmp(cmd.c_str(), "web_dosha") == 0) {
            engine.analyze_doshas(engine.planet_rashis, engine.planet_rashis[0]);
            return 0;
        }
        else if (strcasecmp(cmd.c_str(), "kp") == 0) { engine.calculate_kp(); return 0; }
        else if (strcasecmp(cmd.c_str(), "analyze") == 0) {
            string varga = "D1"; if (clean_argc >= 10) varga = clean_argv[9];
            engine.analyze_chart(varga); return 0;
        }
        else if (strcasecmp(cmd.c_str(), "collision") == 0) {
            if (clean_argc >= 10) {
                string col_planet = clean_argv[9];
                bool col_year_only = false, col_month_only = false;
                if (clean_argc == 11) { t_year = stoi(clean_argv[10]); col_year_only = true; }
                else if (clean_argc == 12) { t_year = stoi(clean_argv[10]); t_month = stoi(clean_argv[11]); col_month_only = true; }
                else if (clean_argc >= 13) { t_year = stoi(clean_argv[10]); t_month = stoi(clean_argv[11]); t_day = stoi(clean_argv[12]); }
                engine.calculate_collisions(col_planet, t_year, t_month, t_day, col_year_only, col_month_only); 
            } else print_help_menu();
            return 0;
        }
        else if (strcasecmp(cmd.c_str(), "daily") == 0) {
            if (clean_argc >= 12) { t_year = stoi(clean_argv[9]); t_month = stoi(clean_argv[10]); t_day = stoi(clean_argv[11]); }
            else { t_year = year; t_month = month; t_day = day; }
            engine.calculate_muhurat(t_year, t_month, t_day, true);
            engine.calculate_daily_panchang_transitions(t_year, t_month, t_day);
            engine.calculate_daily_lagnas(t_year, t_month, t_day); 
            engine.calculate_daily_horas(t_year, t_month, t_day); 
            return 0;
        }
        else if (strcasecmp(cmd.c_str(), "dasha") == 0) {
            if (clean_argc >= 10 && strcasecmp(clean_argv[9], "all") == 0) engine.export_all_dashas_csv();
            else if (clean_argc >= 12) { 
                t_year = stoi(clean_argv[9]); t_month = stoi(clean_argv[10]); t_day = stoi(clean_argv[11]); parse_target_time(12);
                engine.calculate_muhurat(t_year, t_month, t_day, true);
                engine.calculate_daily_panchang_transitions(t_year, t_month, t_day);
                engine.calculate_6_level_dasha_target(t_year, t_month, t_day, t_hour, t_min, t_sec, false);
            }
            else { engine.calculate_dasha_balance(); engine.interactive_dasha(); }
            return 0;
        }
        else if (strcasecmp(cmd.c_str(), "deha") == 0) {
            if (clean_argc >= 12) { 
                t_year = stoi(clean_argv[9]); t_month = stoi(clean_argv[10]); t_day = stoi(clean_argv[11]); parse_target_time(12);
                engine.calculate_muhurat(t_year, t_month, t_day, true);
                engine.calculate_daily_panchang_transitions(t_year, t_month, t_day);
                engine.calculate_daily_dehas(t_year, t_month, t_day, t_hour, t_min, t_sec, false, time_provided);
            } else {
                time_t t = time(nullptr); tm* now = localtime(&t);
                engine.calculate_muhurat(now->tm_year + 1900, now->tm_mon + 1, now->tm_mday, true);
                engine.calculate_daily_panchang_transitions(now->tm_year + 1900, now->tm_mon + 1, now->tm_mday);
                engine.calculate_daily_dehas(0, 0, 0, 0, 0, 0, true, false);
            }
            return 0;
        }
		else if (strcasecmp(cmd.c_str(), "web_transit") == 0) {
            if (clean_argc >= 12) { 
                t_year = stoi(clean_argv[9]); t_month = stoi(clean_argv[10]); t_day = stoi(clean_argv[11]); parse_target_time(12);
                engine.calculate_muhurat(t_year, t_month, t_day, true);
                engine.calculate_daily_panchang_transitions(t_year, t_month, t_day);
                // Passing 'true' at the end tells it to skip Phase 4 and 5
                engine.calculate_transits(t_year, t_month, t_day, t_hour, t_min, t_sec, false, true); 
            }
            return 0;
        }
        else if (strcasecmp(cmd.c_str(), "transit") == 0) {
            if (clean_argc >= 12) { 
                t_year = stoi(clean_argv[9]); t_month = stoi(clean_argv[10]); t_day = stoi(clean_argv[11]); parse_target_time(12);
                engine.calculate_muhurat(t_year, t_month, t_day, true);
                engine.calculate_daily_panchang_transitions(t_year, t_month, t_day);
                // Passing 'false' runs the full heavy calculation for the CLI
                engine.calculate_transits(t_year, t_month, t_day, t_hour, t_min, t_sec, false, false); 
            } else {
                time_t t = time(nullptr); tm* now = localtime(&t);
                engine.calculate_muhurat(now->tm_year + 1900, now->tm_mon + 1, now->tm_mday, true);
                engine.calculate_daily_panchang_transitions(now->tm_year + 1900, now->tm_mon + 1, now->tm_mday);
                engine.calculate_transits(0, 0, 0, 0, 0, 0, true, false);
            }
            return 0;
        }
        else if (strcasecmp(cmd.c_str(), "degree") == 0) {
            if (clean_argc >= 13) {
                string s_planet = clean_argv[9], s_sign = clean_argv[10];
                int s_deg = stoi(clean_argv[11]), s_min = stoi(clean_argv[12]), s_sec = stoi(clean_argv[13]);
                int s_year = 0, s_month = 0;
                if (clean_argc >= 15) s_year = stoi(clean_argv[14]);
                if (clean_argc >= 16) s_month = stoi(clean_argv[15]);
                engine.search_exact_degree(s_planet, s_sign, s_deg, s_min, s_sec, s_year, s_month);
            } else printf("Error: 'degree' requires Planet, Sign, Deg, Min, Sec. Example: degree ravi meena 13 11 00\n");
            return 0;
        }
        else if (strcasecmp(cmd.c_str(), "tithi") == 0) {
            if (clean_argc >= 10) engine.calculate_tithi_return(stoi(clean_argv[9]));
            else printf("Error: 'tithi' requires a target year. Example: tithi 2026\n");
            return 0;
        }
        else if (strcasecmp(cmd.c_str(), "match") == 0 || strcasecmp(cmd.c_str(), "match_predict") == 0) {
            if (clean_argc >= 16) {
                int start_y = 0, end_y = 0, m_idx = 9;
                bool is_predict = (strcasecmp(cmd.c_str(), "match_predict") == 0);
                if (is_predict) {
                    if (clean_argc < 18) { printf("Error: match_predict requires StartY EndY P2_Y P2_M P2_D P2_H P2_Min P2_S P2_City\n"); return 1; }
                    start_y = stoi(clean_argv[m_idx++]); end_y = stoi(clean_argv[m_idx++]);
                }
                int m_y = stoi(clean_argv[m_idx++]), m_m = stoi(clean_argv[m_idx++]), m_d = stoi(clean_argv[m_idx++]);
                int m_h = stoi(clean_argv[m_idx++]), m_min = stoi(clean_argv[m_idx++]), m_s = stoi(clean_argv[m_idx++]);
                string m_city = clean_argv[m_idx++];
                
                auto it2 = find_if(city_db.begin(), city_db.end(), [&](const City& c) { return strcasecmp(c.name.c_str(), m_city.c_str()) == 0; });
                if (it2 == city_db.end()) { printf("Error: Person 2 City '%s' not found.\n", m_city.c_str()); return 1; }
                
                JyotishaEngine p2_engine(m_y, m_m, m_d, m_h, m_min, m_s, *it2, json_mode, telugu_ui);
                p2_engine.calculate_chart();
                
                if (is_predict) predict_synastry_events(engine, p2_engine, start_y, end_y);
                else calculate_synastry(engine, p2_engine);
            } else printf("Error: Missing parameters for match command.\n");
            return 0;
        }
        else if (strcasecmp(cmd.c_str(), "annual") == 0) {
            if (clean_argc >= 10) {
                int annual_year = stoi(clean_argv[9]);
                double tithi_jd = engine.calculate_tithi_return(annual_year);
                if (tithi_jd > 0.0) {
                    int ty, tm, td; double jut; 
                    swe_revjul(tithi_jd + (engine.location.tz_offset / 24.0), SE_GREG_CAL, &ty, &tm, &td, &jut);
                    int th = (int)jut; int tmin = (int)((jut - th) * 60.0); int tsec = (int)((((jut - th) * 60.0) - tmin) * 60.0);
                    
                    printf("\n=================================================================\n");
                    printf("=== VARSHA KUNDALI (ANNUAL CHART FOR %d) ===\n", annual_year);
                    printf("=================================================================\n");
                    
                    JyotishaEngine annual_engine(ty, tm, td, th, tmin, tsec, *it, json_mode, telugu_ui);
                    annual_engine.calculate_chart();
                    annual_engine.analyze_chart("D1");
                }
            } else printf("Error: 'annual' requires a target year.\n");
            return 0;
        }
        else if (strcasecmp(cmd.c_str(), "predict") == 0) {
            if (clean_argc >= 11) engine.predict_marriage(stoi(clean_argv[9]), stoi(clean_argv[10]));
            else printf("Error: 'predict' requires Start Year and End Year.\n");
            return 0;
        }
        else if (strcasecmp(cmd.c_str(), "ayush") == 0) { run_ayush_analysis(engine); return 0; }
        else if (strcasecmp(cmd.c_str(), "muhurat") == 0) {
            if (clean_argc >= 12) {
                engine.calculate_event_muhurat(clean_argv[9], stoi(clean_argv[10]), stoi(clean_argv[11]));
            } else printf("Error: 'muhurat' requires Event, Year, Month. Ex: muhurat marriage 2026 6\n");
            return 0;
        }
        else if (strcasecmp(cmd.c_str(), "all") == 0) {
            string target_planet_all = "all";
            if (clean_argc > 9 && !isdigit(clean_argv[9][0])) {
                target_planet_all = clean_argv[9];
                if (clean_argc > 10) t_year = stoi(clean_argv[10]);
                if (clean_argc > 11) t_month = stoi(clean_argv[11]);
                if (clean_argc > 12) t_day = stoi(clean_argv[12]);
            } else if (clean_argc > 9) {
                t_year = stoi(clean_argv[9]);
                if (clean_argc > 10) t_month = stoi(clean_argv[10]);
                if (clean_argc > 11) t_day = stoi(clean_argv[11]);
            }

            engine.calculate_navatara_table();
            engine.calculate_special_karakas();
            engine.calculate_muhurat(year, month, day, true); 
            engine.calculate_daily_panchang_transitions(year, month, day);
            
            int varsha_lord_idx = -1, masa_lord_idx = -1;
            engine.calculate_varsha_masa(varsha_lord_idx, masa_lord_idx);
            ShadbalaEngine::calculate(engine.lagna_lon, engine.planet_lons, engine.moon_lon, engine.tjd_ut, 
                                      engine.local_hour_decimal, engine.sunrise_hour_decimal, engine.sunset_hour_decimal, 
                                      engine.current_weekday, varsha_lord_idx, masa_lord_idx, false, engine.json_output);
            
            engine.calculate_aspects();
            engine.calculate_shodashvarga(); 
            engine.calculate_ashtakavarga();
            engine.calculate_panchang();
            
            int target_year = (t_year > 0) ? t_year : year; 
            engine.calculate_dasha_balance();
            engine.calculate_6_level_dasha_target(0, 0, 0, 12, 0, 0, true);
            engine.analyze_chart("D1");
            engine.scan_planetary_collisions(target_planet_all, target_year, t_month, t_day);
            return 0;
        }
        else { print_help_menu(); return 1; }
    }

    // Default action if no command is provided
    print_help_menu();
    return 0;
}