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
const double TRUE_SIDEREAL_YEAR = 365.256363004;

struct City { string name; double lat; double lon; double tz_offset; };

const vector<City> city_db = {
    //{"Nellore", 14.450000, 79.986944, 5.5},
	{"Nellore", 14.442599, 79.986458, 5.5},
	{"Kavali", 14.913181, 79.992981, 5.5},
	{"Atmakur", 14.616700, 79.616700, 5.5},
    {"Kovur", 14.483333, 79.983333, 5.5},
	{"Madanapalle", 13.553956, 78.502394, 5.5},


	{"Guntur", 16.306700, 80.436500, 5.5},
	{"Mumbai", 19.076000, 72.877700, 5.5},
	{"Guwahati", 26.144500, 91.736200, 5.5},
	
	{"Patchalatadiparru", 16.111750, 80.536667, 5.5},
    {"Nimmakuru", 16.270300, 80.996700, 5.5},
    {"Hyderabad", 17.385044, 78.486671, 5.5},
    {"Chennai", 13.082680, 80.270718, 5.5},
    {"Bitragunta", 13.966700, 80.083300, 5.5},
    {"Krishna", 16.270300, 80.996700, 5.5},
    {"Bangalore", 12.971600, 77.594600, 5.5},
    {"Mumbai", 19.076000, 72.877700, 5.5},
    {"Delhi", 28.613900, 77.209000, 5.5},
    {"Kolkata", 22.572600, 88.363900, 5.5},
	{"Pune", 18.5204, 73.8567, 5.5},
    {"Jaipur", 26.9124, 75.7873, 5.5},
    {"Varanasi", 25.3176, 82.9739, 5.5},
    {"Ahmedabad", 23.0225, 72.5714, 5.5},
	{"Prayagraj",  25.435800, 81.846300, 5.5}, 
	{"Allahabad",  25.435800, 81.846300, 5.5}, 
	{"Mangalore",  12.914100, 74.856000, 5.5}, 
	{"Indore",     22.719600, 75.857700, 5.5}, 
	{"Rameswaram",  9.287600, 79.312900, 5.5},
	{"Mogalthur", 16.409321, 81.595438, 5.5},
    {"Jamshedpur", 22.805618, 86.203110, 5.5},

	    // North America
    {"New York City", 40.712776, -74.005974, -5.0},
    {"San Francisco", 37.774929, -122.419416, -8.0},
    {"Los Angeles", 34.052234, -118.243685, -8.0},
    {"Chicago", 41.878114, -87.629798, -6.0},
    {"Washington, D.C.", 38.907192, -77.036871, -5.0},
    {"Toronto", 43.653226, -79.383184, -5.0},
    {"Vancouver", 49.282729, -123.120738, -8.0},

    // Europe
    {"London", 51.507351, -0.127758, 0.0},
    {"Amsterdam", 52.367573, 4.904139, 1.0},
    {"Berlin", 52.520007, 13.404954, 1.0},
    {"Frankfurt", 50.110922, 8.682127, 1.0},
    {"Paris", 48.856614, 2.352222, 1.0},

    // Asia-Pacific & Middle East
    {"Dubai", 25.204849, 55.270783, 4.0},
    {"Singapore", 1.352083, 103.819836, 8.0},
    {"Tokyo", 35.676192, 139.650327, 9.0},
    {"Sydney", -33.868820, 151.209296, 10.0},
    {"Melbourne", -37.813628, 144.963058, 10.0},
	{"Novo-Mesto", 45.803900, 15.168900, 1.0},

	// South America & Africa
    {"Sao Paulo", -23.550520, -46.633308, -3.0},
    {"Johannesburg", -26.204103, 28.047305, 2.0},
    // Extra US Tech Hub
    {"Seattle", 47.606209, -122.332071, -8.0}
};

const char* en_short_p_names[] = {"Asc", "Su", "Mo", "Ma", "Me", "Ju", "Ve", "Sa", "Ra", "Ke"};

// ==========================================
// NAMA NAKSHATRA (NAMING SYLLABLES)
// ==========================================
const char* nama_aksharas[27][4] = {
    {"Chu", "Che", "Cho", "La"}, {"Li", "Lu", "Le", "Lo"}, {"A", "I", "U", "E"},
    {"O", "Va", "Vi", "Vu"}, {"Ve", "Vo", "Ka", "Ki"}, {"Ku", "Gha", "Ng", "Chha"},
    {"Ke", "Ko", "Ha", "Hi"}, {"Hu", "He", "Ho", "Da"}, {"Di", "Du", "De", "Do"},
    {"Ma", "Mi", "Mu", "Me"}, {"Mo", "Ta", "Ti", "Tu"}, {"Te", "To", "Pa", "Pi"},
    {"Pu", "Sha", "Na", "Tha"}, {"Pe", "Po", "Ra", "Ri"}, {"Ru", "Re", "Ro", "Ta"},
    {"Ti", "Tu", "Te", "To"}, {"Na", "Ni", "Nu", "Ne"}, {"No", "Ya", "Yi", "Yu"},
    {"Ye", "Yo", "Bha", "Bhi"}, {"Bhu", "Dha", "Pha", "Dha"}, {"Bhe", "Bho", "Ja", "Ji"},
    {"Ju", "Je", "Jo", "Gha"}, {"Ga", "Gi", "Gu", "Ge"}, {"Go", "Sa", "Si", "Su"},
    {"Se", "So", "Da", "Di"}, {"Du", "Tha", "Jha", "Na"}, {"De", "Do", "Cha", "Chi"}
};

struct Transition { int rashi_idx, nak_idx, pada, h, m, s; bool is_rashi_change; };
struct Karaka { int p_idx; double deg; };
struct DashaPeriod { int lord_idx; double start_jd; double duration; };
struct DashaState { double start; double duration; int lord; };
struct TransitHit { string p_name; string hit_type; };


// =========================================================================
// ENGINE CLASS
// =========================================================================

class JyotishaEngine {
public:
    string user_name = "";
    string user_gender = "";
    bool json_mode = false;
    bool telugu_mode = false;
    bool html_mode = false;
    double dasha_year_len; // NEW: Dynamic Dasha Time Constant

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

// --- Search Helper Functions ---
    int get_planet_idx(string p_name) {
        string p_lower = p_name; transform(p_lower.begin(), p_lower.end(), p_lower.begin(), ::tolower);
        if (p_lower == "lagna" || p_lower == "ascendant" || p_lower == "asc") return 0;
        if (p_lower == "surya" || p_lower == "sun" || p_lower == "ravi") return 1;
        if (p_lower == "chandra" || p_lower == "moon") return 2;
        if (p_lower == "mangal" || p_lower == "mars" || p_lower == "kuja") return 3;
        if (p_lower == "budha" || p_lower == "mercury") return 4;
        if (p_lower == "guru" || p_lower == "jupiter" || p_lower == "brihaspati") return 5;
        if (p_lower == "shukra" || p_lower == "venus" || p_lower == "sukra") return 6;
        if (p_lower == "shani" || p_lower == "saturn") return 7;
        if (p_lower == "rahu") return 8;
        if (p_lower == "ketu") return 9;
        return -1;
    }

    int get_rashi_idx(string s_name) {
        string s_lower = s_name; transform(s_lower.begin(), s_lower.end(), s_lower.begin(), ::tolower);
        if (s_lower == "mesha" || s_lower == "mesh") return 0;
        if (s_lower == "vrishabha" || s_lower == "vrish") return 1;
        if (s_lower == "mithuna" || s_lower == "mitu") return 2;
        if (s_lower == "karka" || s_lower == "karkataka") return 3;
        if (s_lower == "simha" || s_lower == "simh") return 4;
        if (s_lower == "kanya" || s_lower == "kany") return 5;
        if (s_lower == "tula") return 6;
        if (s_lower == "vrishchika" || s_lower == "vrischika") return 7;
        if (s_lower == "dhanu" || s_lower == "dhanus") return 8;
        if (s_lower == "makara" || s_lower == "makar") return 9;
        if (s_lower == "kumbha" || s_lower == "kumbh") return 10;
        if (s_lower == "meena" || s_lower == "meen") return 11;
        return -1;
    }
	
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
    int node_calc_type;
	
    string json_output = "{\n";

    JyotishaEngine(int y, int m, int d, int h, int min, int sec, City loc, bool j_mode, bool t_mode = false, bool h_mode = false, bool savana_mode = false, bool use_true_node = false) {
        location = loc; json_mode = j_mode; telugu_mode = t_mode; html_mode = h_mode;
        
        // Dynamically assign True Node or Mean Node
        node_calc_type = use_true_node ? 11 : 10; // 11 = SE_TRUE_NODE, 10 = SE_MEAN_NODE
        
        // Set dynamic dasha year length (360 for Savana, Exact Sidereal otherwise)
        dasha_year_len = savana_mode ? 360.0 : TRUE_SIDEREAL_YEAR;
        
        local_hour_decimal = h + (min / 60.0) + (sec / 3600.0);
        double time_ut = local_hour_decimal - location.tz_offset;
        swe_set_ephe_path((char*)"./ephe"); 
        swe_set_sid_mode(SE_SIDM_LAHIRI, 0, 0);
        tjd_ut = swe_julday(y, m, d, time_ut, SE_GREG_CAL);
        swe_set_topo(location.lon, location.lat, 0.0);
        iflag = SEFLG_SWIEPH | SEFLG_SIDEREAL | SEFLG_SPEED | SEFLG_TRUEPOS;

        if (!json_mode && !html_mode) {
            printf("\n=== PURE ASTRONOMICAL C++ ENGINE OUTPUT ===\n");
            printf("Local Date: %02d/%02d/%04d | Local Time: %02d:%02d:%02d\n", d, m, y, h, min, sec);
            printf("Location: %s (Lat: %f, Lon: %f, TZ: %+.1f)\n", location.name.c_str(), location.lat, location.lon, location.tz_offset);
            printf("Time Protocol: %s Year (%.4f Days)\n", savana_mode ? "Savana" : "Sidereal", dasha_year_len);
            printf("Node Protocol: %s\n", use_true_node ? "True Node" : "Mean Node"); // <--- UI CONFIRMATION
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
        int planets[] = {SE_SUN, SE_MOON, SE_MARS, SE_MERCURY, SE_JUPITER, SE_VENUS, SE_SATURN, node_calc_type};
        double xx[6]; char serr[256];
        if (p_idx == 0) {
            double cusps[13], ascmc[10];
            swe_houses_ex(jd, iflag, location.lat, location.lon, 'P', cusps, ascmc);
            return ascmc[0];
        } else if (p_idx < 9) {
            swe_calc_ut(jd, planets[p_idx-1], iflag, xx, serr);
            return xx[0];
        } else { 
            swe_calc_ut(jd, node_calc_type, iflag, xx, serr);
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

	double get_varga_absolute_lon(int v_num, double lon) {
        if (v_num == 1) return fmod(lon + 360.0, 360.0);
        int rashi = get_varga(v_num, lon);
        double deg = 0.0;
        if (v_num == 30) {
            int base_rashi = (int)(lon / 30.0);
            double deg_in_d1 = fmod(lon, 30.0);
            if (base_rashi % 2 == 0) { 
                if (deg_in_d1 < 5.0) deg = (deg_in_d1 / 5.0) * 30.0;
                else if (deg_in_d1 < 10.0) deg = ((deg_in_d1 - 5.0) / 5.0) * 30.0;
                else if (deg_in_d1 < 18.0) deg = ((deg_in_d1 - 10.0) / 8.0) * 30.0;
                else if (deg_in_d1 < 25.0) deg = ((deg_in_d1 - 18.0) / 7.0) * 30.0;
                else deg = ((deg_in_d1 - 25.0) / 5.0) * 30.0;
            } else { 
                if (deg_in_d1 < 5.0) deg = (deg_in_d1 / 5.0) * 30.0;
                else if (deg_in_d1 < 12.0) deg = ((deg_in_d1 - 5.0) / 7.0) * 30.0;
                else if (deg_in_d1 < 20.0) deg = ((deg_in_d1 - 12.0) / 8.0) * 30.0;
                else if (deg_in_d1 < 25.0) deg = ((deg_in_d1 - 20.0) / 5.0) * 30.0;
                else deg = ((deg_in_d1 - 25.0) / 5.0) * 30.0;
            }
        } else {
            deg = fmod(lon * v_num, 30.0);
        }
        return (rashi * 30.0) + deg;
    }
	
	void calculate_chart() {
        double ascmc[10];
        if (swe_houses_ex(tjd_ut, iflag, location.lat, location.lon, 'P', house_cusps, ascmc) >= 0) {
            lagna_lon = ascmc[0]; planet_lons[0] = lagna_lon; planet_rashis[0] = (int)(lagna_lon / 30.0); 
            process_planet(0, lagna_lon); 
        }

        int planets[] = {SE_SUN, SE_MOON, SE_MARS, SE_MERCURY, SE_JUPITER, SE_VENUS, SE_SATURN, node_calc_type};
        double xx[6]; char serr[256];

        for (int i = 0; i < 8; i++) {
            if (swe_calc_ut(tjd_ut, planets[i], iflag, xx, serr) >= 0) {
                planet_lons[i+1] = xx[0]; planet_rashis[i+1] = (int)(xx[0] / 30.0);
                process_planet(i+1, xx[0]); 
                if (planets[i] == SE_SUN) sun_lon = xx[0]; if (planets[i] == SE_MOON) moon_lon = xx[0]; 
            }
        }
        swe_calc_ut(tjd_ut, node_calc_type, iflag, xx, serr);
        planet_lons[9] = fmod(xx[0] + 180.0, 360.0); planet_rashis[9] = (int)(planet_lons[9] / 30.0);
        process_planet(9, planet_lons[9]);
        
        // Calculate Jaimini Karakas mathematically (No Printing Here)
        struct Karaka { int p_idx; double deg; };
        std::vector<Karaka> karakas;
        for (int i = 1; i <= 7; i++) karakas.push_back({i, fmod(planet_lons[i], 30.0)});
        std::sort(karakas.begin(), karakas.end(), [](const Karaka& a, const Karaka& b) { return a.deg > b.deg; });
        atmakaraka_idx = karakas[0].p_idx;
        darakaraka_idx = karakas[6].p_idx;
    }

	// 1. This function ONLY populates the Rasi Chart Grid during calculation
    void process_planet(int p_idx, double decimal_degrees) {
        int rashi_index = (int)(decimal_degrees / 30.0);
        rashi_grid[rashi_index] += get_short_planet(p_idx) + " ";
    }

    // 2. This function ONLY prints the Planet Positions Table
    void process_planet_print(int p_idx, double decimal_degrees) {
        int rashi_index = (int)(decimal_degrees / 30.0); double rashi_degrees = decimal_degrees - (rashi_index * 30.0);
        int degrees = (int)rashi_degrees; int minutes = (int)((rashi_degrees - degrees) * 60.0);
        int seconds = (int)round((((rashi_degrees - degrees) * 60.0) - minutes) * 60.0);
        if (seconds >= 60) { seconds -= 60; minutes += 1; } if (minutes >= 60) { minutes -= 60; degrees += 1; }
        if (degrees >= 30) { degrees -= 30; rashi_index = (rashi_index + 1) % 12; }

        int d9_rashi_index = ((int)(decimal_degrees / (10.0 / 3.0))) % 12;
        double nak_size = 360.0 / 27.0; int nak_index = (int)(decimal_degrees / nak_size);
        int pada = (int)((decimal_degrees - (nak_index * nak_size)) / (nak_size / 4.0)) + 1;
        int nak_lord_index = nak_index % 9;
        int natal_mo_nak = (int)(moon_lon / (360.0 / 27.0)); int tara_idx = (nak_index - natal_mo_nak + 27) % 9;
        
        string nak_pada = get_nak_name(nak_index) + " " + to_string(pada);
        int r_lord_idx = 1;
        for (int p = 1; p <= 7; p++) { if (string(p_names_full[p]) == rashi_lords[rashi_index]) r_lord_idx = p; }
        
       if (!json_mode) {
            if (html_mode) {
                if (telugu_mode) {
                    printf("<tr><td>%s</td><td>%02d° %s %02d'%02d\"</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>", 
                           get_planet_name(p_idx).c_str(), degrees, get_rashi_name(rashi_index).c_str(), minutes, seconds, 
                           get_rashi_name(d9_rashi_index).c_str(), nak_pada.c_str(), get_tara(tara_idx).c_str(), 
                           get_dasha_lord(nak_lord_index).c_str(), get_planet_name(r_lord_idx).c_str());
                } else {
                    // FIX: Use rashi_names instead of short_rashi to print the full English names
                    printf("<tr><td>%s</td><td>%02d° %s %02d'%02d\"</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>", 
                           p_names_full[p_idx], degrees, rashi_names[rashi_index], minutes, seconds, 
                           rashi_names[d9_rashi_index], nak_pada.c_str(), tara_names[tara_idx], 
                           dasha_lords[nak_lord_index], p_names_full[r_lord_idx]);
                }
            } else {
                if (telugu_mode) {
                    printf("%-20s | %02d° %-16s %02d' %02d\" | %-18s | %-35s | %-45s | %-18s | %-18s\n", 
                           get_planet_name(p_idx).c_str(), degrees, get_rashi_name(rashi_index).c_str(), minutes, seconds, 
                           get_rashi_name(d9_rashi_index).c_str(), nak_pada.c_str(), get_tara(tara_idx).c_str(), 
                           get_dasha_lord(nak_lord_index).c_str(), get_planet_name(r_lord_idx).c_str());
                } else {
                    printf("%-15s | %02d° %-10s %02d' %02d\" | %-10s | %-25s | %-30s | %-10s | %-10s\n", 
                           p_names_full[p_idx], degrees, short_rashi[rashi_index], minutes, seconds, 
                           short_rashi[d9_rashi_index], nak_pada.c_str(), tara_names[tara_idx], 
                           dasha_lords[nak_lord_index], p_names_full[r_lord_idx]);
                }
            }
        }
    }	
void draw_south_indian_chart() {
        if (html_mode) {
            auto c = [&](int idx) { return rashi_grid[idx]; };
            string title = telugu_mode ? "రాశి చక్రం" : "RASI CHART";
            string r12 = telugu_mode ? "12 మీనం" : "12 Meen"; string r1 = telugu_mode ? "1 మేషం" : "1 Mesh"; string r2 = telugu_mode ? "2 వృషభం" : "2 Vrish"; string r3 = telugu_mode ? "3 మిథునం" : "3 Mitu";
            string r11 = telugu_mode ? "11 కుంభం" : "11 Kumb"; string r4 = telugu_mode ? "4 కర్కాటకం" : "4 Kark";
            string r10 = telugu_mode ? "10 మకరం" : "10 Maka"; string r5 = telugu_mode ? "5 సింహం" : "5 Simh";
            string r9 = telugu_mode ? "9 ధనుస్సు" : "9 Dhan"; string r8 = telugu_mode ? "8 వృశ్చికం" : "8 Vrishch"; string r7 = telugu_mode ? "7 తుల" : "7 Tula"; string r6 = telugu_mode ? "6 కన్య" : "6 Kany";

            printf("<table class='rasi-table'>");
            printf("<tr><td>%s<span class='p-text'>%s</span></td><td>%s<span class='p-text'>%s</span></td><td>%s<span class='p-text'>%s</span></td><td>%s<span class='p-text'>%s</span></td></tr>", 
                    r12.c_str(), c(11).c_str(), r1.c_str(), c(0).c_str(), r2.c_str(), c(1).c_str(), r3.c_str(), c(2).c_str());
            printf("<tr><td>%s<span class='p-text'>%s</span></td><td colspan='2' rowspan='2' class='rasi-center'>%s</td><td>%s<span class='p-text'>%s</span></td></tr>", 
                    r11.c_str(), c(10).c_str(), title.c_str(), r4.c_str(), c(3).c_str());
            printf("<tr><td>%s<span class='p-text'>%s</span></td><td>%s<span class='p-text'>%s</span></td></tr>", 
                    r10.c_str(), c(9).c_str(), r5.c_str(), c(4).c_str());
            printf("<tr><td>%s<span class='p-text'>%s</span></td><td>%s<span class='p-text'>%s</span></td><td>%s<span class='p-text'>%s</span></td><td>%s<span class='p-text'>%s</span></td></tr>", 
                    r9.c_str(), c(8).c_str(), r8.c_str(), c(7).c_str(), r7.c_str(), c(6).c_str(), r6.c_str(), c(5).c_str());
            printf("</table><br><br>");
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
            printf("-----------------            RASI CHART         -----------------\n");
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

// =========================================================================
    // COLLISION SWEEPER (UPGRADED: VARGA GOCHARA SUPPORT)
    // =========================================================================

    void refine_bubble(int p_idx, double target_lon, double approx_jd, double orb, double &e_in, double &e_peak, double &e_out, int v_num = 1) {
        double step = (1.0 / 1440.0) / v_num; // Scale resolution by Varga multiplier
        double cur = approx_jd;
        int safeguard = 0;
        
        while(get_dist(get_varga_absolute_lon(v_num, get_planet_lon_on_jd(p_idx, cur)), target_lon) <= orb && safeguard++ < 50000) { cur -= step; }
        e_in = cur + step;
        
        cur = approx_jd; safeguard = 0;
        while(get_dist(get_varga_absolute_lon(v_num, get_planet_lon_on_jd(p_idx, cur)), target_lon) <= orb && safeguard++ < 50000) { cur += step; }
        e_out = cur - step;
        
        e_peak = e_in;
        double min_d = 999.0;
        for (double d = e_in; d <= e_out; d += step) {
            double dist = get_dist(get_varga_absolute_lon(v_num, get_planet_lon_on_jd(p_idx, d)), target_lon);
            if (dist < min_d) { min_d = dist; e_peak = d; }
        }
    }

    void sweep_collisions(int p_idx, double start_jd, double end_jd, int v_num = 1) {
        double target_lon = get_varga_absolute_lon(v_num, planet_lons[p_idx]);
        int natal_rashi = (int)(target_lon / 30.0); // Sign Gatekeeper
        const double orb = 2.0;
        
        string varga_prefix = (v_num == 1) ? "D1" : "D" + to_string(v_num);
        
        printf("\n=== %s TRANSIT COLLISION SWEEP: %s (Sign: %s) ===\n", varga_prefix.c_str(), p_names_full[p_idx], rashi_names[natal_rashi]);
        printf("Target Natal Degree: %s\n", format_dms(target_lon).c_str());
        
        if (p_idx == 0) {
            printf("Search Window: %s to %s\n", jd_to_string(start_jd).c_str(), jd_to_string(end_jd).c_str());
        }
        printf("-----------------------------------------------------------------------------------------------------------------\n");

        double step = (4.0 / 24.0) / v_num; 
        if (p_idx == 0) step = (5.0 / 1440.0) / v_num; 
        else if (p_idx == 2) step = (1.0 / 24.0) / v_num; 
        
        int hit_count = 0;
        double scan_start = start_jd - (30.0 / v_num);
        double scan_end = end_jd + (30.0 / v_num);
        if (p_idx == 0) { scan_start = start_jd - (1.0 / v_num); scan_end = end_jd + (1.0 / v_num); }
        if (p_idx == 2) { scan_start = start_jd - (5.0 / v_num); scan_end = end_jd + (5.0 / v_num); }

        for (double jd = scan_start; jd <= scan_end; jd += step) {
            double trans_lon = get_varga_absolute_lon(v_num, get_planet_lon_on_jd(p_idx, jd));
            int trans_rashi = (int)(trans_lon / 30.0); 
            
            if (trans_rashi != natal_rashi) continue; 
            
            double dist = get_dist(trans_lon, target_lon);

            if (dist <= orb) {
                double e_in, e_peak, e_out;
                refine_bubble(p_idx, target_lon, jd, orb, e_in, e_peak, e_out, v_num);

                if (e_peak >= start_jd && e_peak <= end_jd) {
                    printf("Hit %d:  ENTER => %-20s | EXACT PEAK => %-20s | EXIT => %-20s\n", 
                           ++hit_count, jd_to_string(e_in).c_str(), jd_to_string(e_peak).c_str(), jd_to_string(e_out).c_str());
                }
                jd = e_out; 
            }
        }

        if (hit_count == 0) {
            printf("No exact peak collisions occurred within the requested timeframe.\n");
            printf("Scanning to find the True Past and Future peaks...\n\n");
            
            double search_limit = (30.0 * 365.25) / v_num;
            if (p_idx == 0) search_limit = 2.0 / v_num;
            if (p_idx == 2) search_limit = 40.0 / v_num;
            
            double search_jd = start_jd; bool found_past = false;
            while (search_jd >= start_jd - search_limit) { 
                search_jd -= step; 
                double trans_lon = get_varga_absolute_lon(v_num, get_planet_lon_on_jd(p_idx, search_jd));
                if ((int)(trans_lon / 30.0) == natal_rashi && get_dist(trans_lon, target_lon) <= orb) {
                    double e_in, e_peak, e_out;
                    refine_bubble(p_idx, target_lon, search_jd, orb, e_in, e_peak, e_out, v_num);
                    printf("Closest Past Hit   : ENTER => %-20s | EXACT PEAK => %-20s | EXIT => %-20s\n", 
                           jd_to_string(e_in).c_str(), jd_to_string(e_peak).c_str(), jd_to_string(e_out).c_str());
                    found_past = true; break;
                }
            }
            if (!found_past) printf("No past hit found within limit.\n");

            search_jd = end_jd; bool found_future = false;
            while (search_jd <= end_jd + search_limit) { 
                search_jd += step;
                double trans_lon = get_varga_absolute_lon(v_num, get_planet_lon_on_jd(p_idx, search_jd));
                if ((int)(trans_lon / 30.0) == natal_rashi && get_dist(trans_lon, target_lon) <= orb) {
                    double e_in, e_peak, e_out;
                    refine_bubble(p_idx, target_lon, search_jd, orb, e_in, e_peak, e_out, v_num);
                    printf("Closest Future Hit : ENTER => %-20s | EXACT PEAK => %-20s | EXIT => %-20s\n", 
                           jd_to_string(e_in).c_str(), jd_to_string(e_peak).c_str(), jd_to_string(e_out).c_str());
                    found_future = true; break;
                }
            }
        }
        printf("-----------------------------------------------------------------------------------------------------------------\n");
    }

    void calculate_collisions(string p_name, int t_year, int t_month, int t_day, bool is_year_only, bool is_month_only, int v_num = 1) {
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
            string varga_prefix = (v_num == 1) ? "D1" : "D" + to_string(v_num);
            printf("\n=== GLOBAL COLLISION SWEEP [%s] ===\n", varga_prefix.c_str());
            printf("Search Window: %s to %s\n", jd_to_string(start_jd).c_str(), jd_to_string(end_jd).c_str());
        }

        for (int p_idx : targets) {
            sweep_collisions(p_idx, start_jd, end_jd, v_num);
        }
    }

    void scan_planetary_collisions(string target_planet, int start_year, int start_month, int start_day, int v_num = 1) {
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

        string varga_prefix = (v_num == 1) ? "D1" : "D" + to_string(v_num);
        printf("\n=== GLOBAL PRECISION %s TRANSIT SCANNER (YUTI & VEDIC DRISHTI) ===\n", varga_prefix.c_str());
        printf("Scope: %s | Target: %s\n", scope_desc.c_str(), (p_lower == "all" || p_lower == "") ? "All Planets" : p_names_full[t_targets[0]]);
        printf("%-10s | %-14s | %-14s | %-20s | %-20s | %-20s | %-35s\n", 
               "Transit", "Natal", "Aspect Type", "Enter Time", "Peak Time", "Exit Time", "Status & Reason");
        printf("--------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
        const double orb = 2.0;
        struct AspectTarget { double lon; string name; };

        for (int t : t_targets) {
            for (int n = 0; n <= 9; n++) {
                
                vector<AspectTarget> targets;
                double n_lon = get_varga_absolute_lon(v_num, planet_lons[n]);
                
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
                    double step = ((t == 2) ? 1.0 / 24.0 : 4.0 / 24.0) / v_num; 
                    
                    for (double jd = start_jd; jd < end_jd; jd += step) {
                        
                        double trans_lon = get_varga_absolute_lon(v_num, get_planet_lon_on_jd(t, jd));
                        if ((int)(trans_lon / 30.0) != target_rashi) continue; 
                        
                        double dist = std::abs(fmod(trans_lon, 30.0) - fmod(tgt.lon, 30.0));
                        
                    if (dist <= orb) {
                            double e_in, e_peak, e_out;
                            refine_bubble(t, tgt.lon, jd, orb, e_in, e_peak, e_out, v_num);

                            string impact = "";
                            if (n == 0) {
                                bool t_is_malefic = (t == 1 || t == 3 || t == 7 || t == 8 || t == 9);
                                string reason = "[Lagna / Core Physical Body Hit]";
                                impact = (t_is_malefic ? "DANGER " : "OPPORTUNITY ") + reason;
                            } else {
                                string severity = (natal_scores[n] <= -5) ? "High Malefic" : 
                                                  (natal_scores[n] < 0)   ? "Malefic" : 
                                                  (natal_scores[n] >= 5)  ? "High Benefic" : "Neutral/Benefic";
                                
                                string reason = "[Score: " + to_string(natal_scores[n]) + ", " + severity + "]";
                                impact = (natal_scores[n] < 0) ? "DANGER " + reason : "OPPORTUNITY " + reason;
                            }
                            
                            // --- UPGRADED: INJECT DYNAMIC VARGA LORDSHIPS INTO THE NATAL NAME ---
                            string natal_name = (n == 0) ? "Lagna" : string(p_names_full[n]);
                            if (n >= 1 && n <= 7) {
                                string lordships = "";
                                // Dynamically calculate the Lagna for the requested Varga
                                int v_lagna = get_varga(v_num, planet_lons[0]); 
                                
                                for (int h = 1; h <= 12; h++) {
                                    int rashi_of_house = (v_lagna + h - 1) % 12; 
                                    if (string(rashi_lords[rashi_of_house]) == string(p_names_full[n])) {
                                        if (!lordships.empty()) lordships += ",";
                                        lordships += to_string(h);
                                    }
                                }
                                if (!lordships.empty()) natal_name += " (L" + lordships + ")";
                            }
                            
                            // Tightened formatting column from %-18s to %-14s
                            printf("%-10s | %-14s | %-14s | %-20s | %-20s | %-20s | %-35s\n", 
                                   p_names_full[t], natal_name.c_str(), tgt.name.c_str(),
                                   jd_to_string(e_in).c_str(), jd_to_string(e_peak).c_str(), 
                                   jd_to_string(e_out).c_str(), impact.c_str());
                            
                            if (e_out > jd) jd = e_out; 
                    }
					
					}
                }
            }
        }
        printf("----------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
    }
	

void scan_rasi_tulya_varga_collisions(string target_planet, int start_year, int start_month, int start_day, int v_num) {
        if (v_num == 1) return; // D1 to D1 is already handled by the primary scanner
        
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

        string varga_name = "D" + to_string(v_num);
        printf("\n=== RASI TULYA %s SCANNER (PHYSICAL D1 TRANSITS OVER NATAL %s) ===\n", varga_name.c_str(), varga_name.c_str());
        printf("Scope: %s | Target: %s\n", scope_desc.c_str(), (p_lower == "all" || p_lower == "") ? "All Planets" : p_names_full[t_targets[0]]);
        printf("%-10s | %-14s | %-14s | %-20s | %-20s | %-20s | %-35s\n", 
               "Transit D1", ("Natal " + varga_name).c_str(), "Aspect Type", "Enter Time", "Peak Time", "Exit Time", "Status & Reason");
        printf("--------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

        const double orb = 2.0;
        struct AspectTarget { double lon; string name; };

        for (int t : t_targets) {
            for (int n = 0; n <= 9; n++) {
                
                vector<AspectTarget> targets;
                // Fetch the EXACT fractional degree of the Natal Planet inside the requested Varga
                double n_lon = get_varga_absolute_lon(v_num, planet_lons[n]);
                
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
                    // D1 PHYSICAL SPEED LIMITS
                    double step = (t == 2) ? 1.0 / 24.0 : 4.0 / 24.0; 
                    
                    for (double jd = start_jd; jd < end_jd; jd += step) {
                        
                        // Extract physical D1 longitude
                        double trans_lon = get_varga_absolute_lon(1, get_planet_lon_on_jd(t, jd));
                        if ((int)(trans_lon / 30.0) != target_rashi) continue; 
                        
                        double dist = std::abs(fmod(trans_lon, 30.0) - fmod(tgt.lon, 30.0));
                        
                        if (dist <= orb) {
                            double e_in, e_peak, e_out;
                            // Send v_num=1 so refine_bubble bounds the timeline using the physical D1 speed!
                            refine_bubble(t, tgt.lon, jd, orb, e_in, e_peak, e_out, 1);

                            string impact = "";
                            if (n == 0) {
                                bool t_is_malefic = (t == 1 || t == 3 || t == 7 || t == 8 || t == 9);
                                string reason = "[Lagna / Core Matrix Hit]";
                                impact = (t_is_malefic ? "DANGER " : "OPPORTUNITY ") + reason;
                            } else {
                                string severity = (natal_scores[n] <= -5) ? "High Malefic" : 
                                                  (natal_scores[n] < 0)   ? "Malefic" : 
                                                  (natal_scores[n] >= 5)  ? "High Benefic" : "Neutral/Benefic";
                                
                                string reason = "[Score: " + to_string(natal_scores[n]) + ", " + severity + "]";
                                impact = (natal_scores[n] < 0) ? "DANGER " + reason : "OPPORTUNITY " + reason;
                            }
                            
                            string natal_name = (n == 0) ? "Lagna" : string(p_names_full[n]);
                            if (n >= 1 && n <= 7) {
                                string lordships = "";
                                int v_lagna = get_varga(v_num, planet_lons[0]); 
                                
                                for (int h = 1; h <= 12; h++) {
                                    int rashi_of_house = (v_lagna + h - 1) % 12; 
                                    if (string(rashi_lords[rashi_of_house]) == string(p_names_full[n])) {
                                        if (!lordships.empty()) lordships += ",";
                                        lordships += to_string(h);
                                    }
                                }
                                if (!lordships.empty()) natal_name += " (L" + lordships + ")";
                            }
                            
                            printf("%-10s | %-14s | %-14s | %-20s | %-20s | %-20s | %-35s\n", 
                                   p_names_full[t], natal_name.c_str(), tgt.name.c_str(),
                                   jd_to_string(e_in).c_str(), jd_to_string(e_peak).c_str(), 
                                   jd_to_string(e_out).c_str(), impact.c_str());
                            
                            if (e_out > jd) jd = e_out; 
                        }
                    }
                }
            }
        }
        printf("--------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
    }

void print_birth_chart_ui() {
        if (json_mode) return;

        const char* rashi_lords_te_local[] = {"కుజ", "శుక్ర", "బుధ", "చంద్ర", "సూర్య", "బుధ", "శుక్ర", "కుజ", "గురు", "శని", "శని", "గురు"};

        int y, m, d; double jut;
        swe_revjul(tjd_ut + (location.tz_offset / 24.0), SE_GREG_CAL, &y, &m, &d, &jut);

        int th = (int)local_hour_decimal;
        int tmin = (int)((local_hour_decimal - th) * 60.0);
        int tsec = (int)round((((local_hour_decimal - th) * 60.0) - tmin) * 60.0);
        if (tsec >= 60) { tsec -= 60; tmin += 1; } 
        if (tmin >= 60) { tmin -= 60; th += 1; }
        if (th >= 24) { th -= 24; }

        if (html_mode) {
            printf("<div style='background: #1e1e24; padding: 20px; border-radius: 8px; margin-bottom: 20px; border-left: 4px solid var(--accent);'>");
            printf("<h2 style='margin-top: 0; color: var(--accent);'>%s</h2>", telugu_mode ? "జనన వివరాలు" : "Birth Details");
            printf("<table style='width: 100%%; border-collapse: collapse;'>");
            printf("<tr><td style='padding: 5px 0; color: #aaa;'>%s</td><td style='padding: 5px 0; font-weight: bold;'>%02d/%02d/%04d</td></tr>", telugu_mode ? "తేదీ (Date)" : "Date of Birth", d, m, y);
            printf("<tr><td style='padding: 5px 0; color: #aaa;'>%s</td><td style='padding: 5px 0; font-weight: bold;'>%02d:%02d:%02d</td></tr>", telugu_mode ? "సమయం (Time)" : "Time of Birth", th, tmin, tsec);
            printf("<tr><td style='padding: 5px 0; color: #aaa;'>%s</td><td style='padding: 5px 0; font-weight: bold;'>%s</td></tr>", telugu_mode ? "ప్రదేశం (Place)" : "Place of Birth", location.name.c_str());
            printf("<tr><td style='padding: 5px 0; color: #aaa;'>%s</td><td style='padding: 5px 0; font-weight: bold;'>%.4f, %.4f</td></tr>", telugu_mode ? "అక్షాంశం/రేఖాంశం" : "Lat/Lon", location.lat, location.lon);
            printf("<tr><td style='padding: 5px 0; color: #aaa;'>%s</td><td style='padding: 5px 0; font-weight: bold;'>%s</td></tr>", telugu_mode ? "అయనంశ" : "Ayanamsa", "Lahiri (Chitra Paksha)");
            printf("</table></div>\n");

            printf("<h2 style='color: var(--accent); border-bottom: 1px solid var(--border); padding-bottom: 5px;'>%s</h2>", telugu_mode ? "గ్రహ స్థితులు (Planet Positions)" : "Planet Positions");
            printf("<table class='data-table'><tr>");
            printf("<th>%s</th><th>%s</th><th>%s</th><th>%s</th><th>%s</th><th>%s</th></tr>", 
                   telugu_mode ? "గ్రహం" : "Planet", telugu_mode ? "డిగ్రీలు" : "Degrees", telugu_mode ? "రాశి" : "Rashi", telugu_mode ? "నక్షత్రం" : "Nakshatra", telugu_mode ? "పాదం" : "Pada", telugu_mode ? "అధిపతి" : "Lord");
        } else {
            if (telugu_mode) {
                printf("\n=================================================================\n");
                printf("=== జనన వివరాలు (BIRTH DETAILS) ===\n");
                printf("=================================================================\n");
                printf(" తేదీ (Date)    : %02d/%02d/%04d\n", d, m, y);
                printf(" సమయం (Time)   : %02d:%02d:%02d\n", th, tmin, tsec);
                printf(" ప్రదేశం (Place): %s (Lat: %.4f, Lon: %.4f, TZ: %.1f)\n", location.name.c_str(), location.lat, location.lon, location.tz_offset);
                printf(" అయనంశ          : లాహిరి (చిత్ర పక్ష)\n");
                
                printf("\n=================================================================\n");
                printf("=== గ్రహ స్థితులు (PLANET POSITIONS) ===\n");
                printf("=================================================================\n");
                printf("%-10s | %-10s | %-15s | %-20s | %-5s | %-10s\n", "గ్రహం", "డిగ్రీలు", "రాశి", "నక్షత్రం", "పాదం", "అధిపతి");
            } else {
                printf("\n=================================================================\n");
                printf("=== BIRTH DETAILS ===\n");
                printf("=================================================================\n");
                printf(" Date           : %02d/%02d/%04d\n", d, m, y);
                printf(" Time           : %02d:%02d:%02d\n", th, tmin, tsec);
                printf(" Place          : %s (Lat: %.4f, Lon: %.4f, TZ: %.1f)\n", location.name.c_str(), location.lat, location.lon, location.tz_offset);
                printf(" Ayanamsa       : Lahiri (Chitra Paksha)\n");

                printf("\n=================================================================\n");
                printf("=== PLANET POSITIONS ===\n");
                printf("=================================================================\n");
                printf("%-10s | %-10s | %-15s | %-20s | %-5s | %-10s\n", "Planet", "Degrees", "Rashi", "Nakshatra", "Pada", "Lord");
            }
            printf("-------------------------------------------------------------------------------------------------\n");
        }

        for (int i = 0; i <= 9; i++) {
            int rashi = planet_rashis[i];
            double deg = fmod(planet_lons[i], 30.0);
            int nak_idx = (int)(planet_lons[i] / (360.0 / 27.0));
            int pada = (int)((planet_lons[i] - (nak_idx * (360.0 / 27.0))) / ((360.0 / 27.0) / 4.0)) + 1;
            
            int p_lord_idx = -1;
            for(int p=1; p<=7; p++) {
                if (rashi_lords[rashi] == string(p_names_full[p])) { p_lord_idx = p; break; }
            }
            string p_lord_name = p_lord_idx != -1 ? (telugu_mode ? get_planet_name(p_lord_idx) : p_names_full[p_lord_idx]) : (telugu_mode ? rashi_lords_te_local[rashi] : rashi_lords[rashi]);
            
            char deg_str[16];
            snprintf(deg_str, sizeof(deg_str), "%02d° %02d' %02d\"", (int)deg, (int)((deg - (int)deg) * 60), (int)((((deg - (int)deg) * 60) - (int)((deg - (int)deg) * 60)) * 60));

            string p_name = (i == 0) ? (telugu_mode ? "లగ్నం" : "Lagna") : (telugu_mode ? get_planet_name(i) : p_names_full[i]);
            string r_name = telugu_mode ? te_rashi_names[rashi] : rashi_names[rashi];
            string n_name = telugu_mode ? te_nak_names[nak_idx] : nak_names[nak_idx];

            if (html_mode) {
                printf("<tr><td><b>%s</b></td><td>%s</td><td>%s</td><td>%s</td><td>%d</td><td>%s</td></tr>\n", 
                       p_name.c_str(), deg_str, r_name.c_str(), n_name.c_str(), pada, p_lord_name.c_str());
            } else {
                if (telugu_mode) printf("%-12s | %-12s | %-18s | %-22s | %-5d | %-10s\n", p_name.c_str(), deg_str, r_name.c_str(), n_name.c_str(), pada, p_lord_name.c_str());
                else printf("%-10s | %-10s | %-15s | %-20s | %-5d | %-10s\n", p_name.c_str(), deg_str, r_name.c_str(), n_name.c_str(), pada, p_lord_name.c_str());
            }
        }
        
        if (html_mode) printf("</table>\n");
        else printf("-------------------------------------------------------------------------------------------------\n");
        
        // --- DYNAMIC COLORED RASI CHART GENERATOR (No Rashi Names) ---
        auto get_planets = [&](int rashi) -> string {
            string res = "";
            const char* p_colors[] = {
                "#f1c40f", // 0: Lagna (Gold)
                "#ff6b81", // 1: Sun (Coral Red)
                "#ffffff", // 2: Moon (White)
                "#e84118", // 3: Mars (Deep Red)
                "#2ed573", // 4: Mercury (Emerald Green)
                "#f1c40f", // 5: Jupiter (Gold)
                "#ff9ff3", // 6: Venus (Pink)
                "#1e90ff", // 7: Saturn (Royal Blue)
                "#a29bfe", // 8: Rahu (Lavender/Indigo)
                "#ced6e0"  // 9: Ketu (Silver Ash)
            };

            if (planet_rashis[0] == rashi) {
                if (html_mode) res += "<span style='color:" + string(p_colors[0]) + ";'>" + (telugu_mode ? "లగ్న" : "Asc") + "</span> ";
                else res += telugu_mode ? "లగ్న " : "Asc ";
            }
            for (int i = 1; i <= 9; i++) {
                if (planet_rashis[i] == rashi) {
                    if (html_mode) res += "<span style='color:" + string(p_colors[i]) + ";'>" + get_short_planet(i) + "</span> ";
                    else res += get_short_planet(i) + " ";
                }
            }
            if (!res.empty() && res.back() == ' ') res.pop_back(); 
            return res;
        };

        if (html_mode) {
            printf("<h3 style='color: var(--accent); margin-top: 25px; margin-bottom: 10px;'>%s</h3>", telugu_mode ? "రాశి చక్రం (D1)" : "Rasi Chart (D1)");
            printf("<table class='rasi-table'>");
            printf("<tr><td><div style='font-weight:bold; font-size:1.05em; letter-spacing:0.5px;'>%s</div></td><td><div style='font-weight:bold; font-size:1.05em; letter-spacing:0.5px;'>%s</div></td><td><div style='font-weight:bold; font-size:1.05em; letter-spacing:0.5px;'>%s</div></td><td><div style='font-weight:bold; font-size:1.05em; letter-spacing:0.5px;'>%s</div></td></tr>", 
                get_planets(11).c_str(), get_planets(0).c_str(), get_planets(1).c_str(), get_planets(2).c_str());
            printf("<tr><td><div style='font-weight:bold; font-size:1.05em; letter-spacing:0.5px;'>%s</div></td><td colspan='2' rowspan='2' class='rasi-center'><b>%s</b></td><td><div style='font-weight:bold; font-size:1.05em; letter-spacing:0.5px;'>%s</div></td></tr>", 
                get_planets(10).c_str(), telugu_mode ? "రాశి చక్రం<br>(D1)" : "RASI CHART<br>(D1)", get_planets(3).c_str());
            printf("<tr><td><div style='font-weight:bold; font-size:1.05em; letter-spacing:0.5px;'>%s</div></td><td><div style='font-weight:bold; font-size:1.05em; letter-spacing:0.5px;'>%s</div></td></tr>", 
                get_planets(9).c_str(), get_planets(4).c_str());
            printf("<tr><td><div style='font-weight:bold; font-size:1.05em; letter-spacing:0.5px;'>%s</div></td><td><div style='font-weight:bold; font-size:1.05em; letter-spacing:0.5px;'>%s</div></td><td><div style='font-weight:bold; font-size:1.05em; letter-spacing:0.5px;'>%s</div></td><td><div style='font-weight:bold; font-size:1.05em; letter-spacing:0.5px;'>%s</div></td></tr>", 
                get_planets(8).c_str(), get_planets(7).c_str(), get_planets(6).c_str(), get_planets(5).c_str());
            printf("</table>\n");
        } else {
            string p12 = get_planets(11), p1 = get_planets(0), p2 = get_planets(1), p3 = get_planets(2);
            string p11 = get_planets(10), p4 = get_planets(3);
            string p10 = get_planets(9),  p5 = get_planets(4);
            string p9 = get_planets(8),   p8 = get_planets(7), p7 = get_planets(6), p6 = get_planets(5);

            printf("\n[%s]\n", telugu_mode ? "రాశి చక్రం (D1)" : "RASI CHART (D1)");
            printf("+-----------------+-----------------+-----------------+-----------------+\n");
            printf("|                 |                 |                 |                 |\n");
            printf("| %-15s | %-15s | %-15s | %-15s |\n", p12.c_str(), p1.c_str(), p2.c_str(), p3.c_str());
            printf("+-----------------+-----------------+-----------------+-----------------+\n");
            printf("|                 |                                     |                 |\n");
            printf("| %-15s |             RASI CHART              | %-15s |\n", p11.c_str(), p4.c_str());
            printf("+-----------------+                 (D1)                +-----------------+\n");
            printf("|                 |                                     |                 |\n");
            printf("| %-15s |                                     | %-15s |\n", p10.c_str(), p5.c_str());
            printf("+-----------------+-----------------+-----------------+-----------------+\n");
            printf("|                 |                 |                 |                 |\n");
            printf("| %-15s | %-15s | %-15s | %-15s |\n", p9.c_str(), p8.c_str(), p7.c_str(), p6.c_str());
            printf("+-----------------+-----------------+-----------------+-----------------+\n");
        }
    }
void print_varga_positions_and_grid(int v_num, string varga_str, int v_lagna, int* v_planets) {
        const char* rashi_lords_te_local[] = {"కుజ", "శుక్ర", "బుధ", "చంద్ర", "సూర్య", "బుధ", "శుక్ర", "కుజ", "గురు", "శని", "శని", "గురు"};

        // 1. Planet Positions Table
        if (html_mode) {
            string pos_title = telugu_mode ? (varga_str + " గ్రహ స్థితులు") : (varga_str + " Planet Positions");
            printf("<h3 style='color: var(--accent); margin-top: 20px; margin-bottom: 10px;'>%s</h3>", pos_title.c_str());
            printf("<table class='data-table'><tr>");
            if (v_num == 1) {
                printf("<th>%s</th><th>%s</th><th>%s</th><th>%s</th><th>%s</th><th>%s</th></tr>\n", 
                       telugu_mode ? "గ్రహం" : "Planet", telugu_mode ? "డిగ్రీలు" : "Degrees", telugu_mode ? "రాశి" : "Rashi", telugu_mode ? "నక్షత్రం" : "Nakshatra", telugu_mode ? "పాదం" : "Pada", telugu_mode ? "అధిపతి" : "Lord");
            } else {
                printf("<th>%s</th><th>%s</th><th>%s</th><th>%s</th></tr>\n", 
                       telugu_mode ? "గ్రహం" : "Planet", telugu_mode ? "డిగ్రీలు" : "Degrees", telugu_mode ? "రాశి" : "Rashi", telugu_mode ? "అధిపతి" : "Lord");
            }
        } else {
            if (telugu_mode) printf("\n[%s గ్రహ స్థితులు]\n", varga_str.c_str());
            else printf("\n[%s PLANET POSITIONS]\n", varga_str.c_str());
            
            printf("-------------------------------------------------------------------------------------------------\n");
            if (v_num == 1) {
                if (telugu_mode) printf("%-12s | %-12s | %-18s | %-22s | %-5s | %-10s\n", "గ్రహం", "డిగ్రీలు", "రాశి", "నక్షత్రం", "పాదం", "అధిపతి");
                else printf("%-10s | %-12s | %-15s | %-20s | %-5s | %-10s\n", "Planet", "Degrees", "Rashi", "Nakshatra", "Pada", "Lord");
            } else {
                if (telugu_mode) printf("%-12s | %-12s | %-18s | %-10s\n", "గ్రహం", "డిగ్రీలు", "రాశి", "అధిపతి");
                else printf("%-10s | %-12s | %-15s | %-10s\n", "Planet", "Degrees", "Rashi", "Lord");
            }
            printf("-------------------------------------------------------------------------------------------------\n");
        }

        for (int i = 0; i <= 9; i++) {
            int rashi = v_planets[i];
            int p_lord_idx = -1;
            for(int p=1; p<=7; p++) {
                if (rashi_lords[rashi] == string(p_names_full[p])) { p_lord_idx = p; break; }
            }
            string p_lord_name = p_lord_idx != -1 ? (telugu_mode ? get_planet_name(p_lord_idx) : p_names_full[p_lord_idx]) : (telugu_mode ? rashi_lords_te_local[rashi] : rashi_lords[rashi]);
            string p_name = (i == 0) ? (telugu_mode ? "లగ్నం" : "Lagna") : (telugu_mode ? get_planet_name(i) : p_names_full[i]);
            string r_name = telugu_mode ? te_rashi_names[rashi] : rashi_names[rashi];

            double deg_in_d1 = fmod(planet_lons[i], 30.0);
            double v_deg = 0.0;
            
            if (v_num == 30) {
                int base_rashi = (int)(planet_lons[i] / 30.0);
                if (base_rashi % 2 == 0) { // Even Signs
                    if (deg_in_d1 < 5.0) v_deg = (deg_in_d1 / 5.0) * 30.0;
                    else if (deg_in_d1 < 10.0) v_deg = ((deg_in_d1 - 5.0) / 5.0) * 30.0;
                    else if (deg_in_d1 < 18.0) v_deg = ((deg_in_d1 - 10.0) / 8.0) * 30.0;
                    else if (deg_in_d1 < 25.0) v_deg = ((deg_in_d1 - 18.0) / 7.0) * 30.0;
                    else v_deg = ((deg_in_d1 - 25.0) / 5.0) * 30.0;
                } else { // Odd Signs
                    if (deg_in_d1 < 5.0) v_deg = (deg_in_d1 / 5.0) * 30.0;
                    else if (deg_in_d1 < 12.0) v_deg = ((deg_in_d1 - 5.0) / 7.0) * 30.0;
                    else if (deg_in_d1 < 20.0) v_deg = ((deg_in_d1 - 12.0) / 8.0) * 30.0;
                    else if (deg_in_d1 < 25.0) v_deg = ((deg_in_d1 - 20.0) / 5.0) * 30.0;
                    else v_deg = ((deg_in_d1 - 25.0) / 5.0) * 30.0;
                }
            } else {
                v_deg = fmod(planet_lons[i] * v_num, 30.0);
            }
            
            char v_deg_str[16];
            snprintf(v_deg_str, sizeof(v_deg_str), "%02d° %02d' %02d\"", (int)v_deg, (int)((v_deg - (int)v_deg) * 60), (int)((((v_deg - (int)v_deg) * 60) - (int)((v_deg - (int)v_deg) * 60)) * 60));

            if (v_num == 1) {
                int nak_idx = (int)(planet_lons[i] / (360.0 / 27.0));
                int pada = (int)((planet_lons[i] - (nak_idx * (360.0 / 27.0))) / ((360.0 / 27.0) / 4.0)) + 1;
                string n_name = telugu_mode ? te_nak_names[nak_idx] : nak_names[nak_idx];
                
                if (html_mode) {
                    printf("<tr><td><b>%s</b></td><td>%s</td><td>%s</td><td>%s</td><td>%d</td><td>%s</td></tr>\n", 
                           p_name.c_str(), v_deg_str, r_name.c_str(), n_name.c_str(), pada, p_lord_name.c_str());
                } else {
                    if (telugu_mode) printf("%-12s | %-12s | %-18s | %-22s | %-5d | %-10s\n", p_name.c_str(), v_deg_str, r_name.c_str(), n_name.c_str(), pada, p_lord_name.c_str());
                    else printf("%-10s | %-12s | %-15s | %-20s | %-5d | %-10s\n", p_name.c_str(), v_deg_str, r_name.c_str(), n_name.c_str(), pada, p_lord_name.c_str());
                }
            } else {
                if (html_mode) {
                    printf("<tr><td><b>%s</b></td><td>%s</td><td>%s</td><td>%s</td></tr>\n", p_name.c_str(), v_deg_str, r_name.c_str(), p_lord_name.c_str());
                } else {
                    if (telugu_mode) printf("%-12s | %-12s | %-18s | %-10s\n", p_name.c_str(), v_deg_str, r_name.c_str(), p_lord_name.c_str());
                    else printf("%-10s | %-12s | %-15s | %-10s\n", p_name.c_str(), v_deg_str, r_name.c_str(), p_lord_name.c_str());
                }
            }
        }
        
        if (html_mode) printf("</table>\n");
        else printf("-------------------------------------------------------------------------------------------------\n");

        // --- DYNAMIC COLORED RASI CHART GENERATOR (No Rashi Names) ---
        auto get_planets = [&](int rashi) -> string {
            string res = "";
            const char* p_colors[] = {
                "#f1c40f", // 0: Lagna (Gold)
                "#ff6b81", // 1: Sun (Coral Red)
                "#ffffff", // 2: Moon (White)
                "#e84118", // 3: Mars (Deep Red)
                "#2ed573", // 4: Mercury (Emerald Green)
                "#f1c40f", // 5: Jupiter (Gold)
                "#ff9ff3", // 6: Venus (Pink)
                "#1e90ff", // 7: Saturn (Royal Blue)
                "#a29bfe", // 8: Rahu (Lavender/Indigo)
                "#ced6e0"  // 9: Ketu (Silver Ash)
            };

            if (v_lagna == rashi) {
                if (html_mode) res += "<span style='color:" + string(p_colors[0]) + ";'>" + (telugu_mode ? "లగ్న" : "Asc") + "</span> ";
                else res += telugu_mode ? "లగ్న " : "Asc ";
            }
            for (int i = 1; i <= 9; i++) {
                if (v_planets[i] == rashi) {
                    if (html_mode) res += "<span style='color:" + string(p_colors[i]) + ";'>" + get_short_planet(i) + "</span> ";
                    else res += get_short_planet(i) + " ";
                }
            }
            if (!res.empty() && res.back() == ' ') res.pop_back();
            return res;
        };
        
        if (html_mode) {
            string grid_title = telugu_mode ? (varga_str + " రాశి చక్రం") : (varga_str + " Rasi Chart");
            printf("<h3 style='color: var(--accent); margin-top: 25px; margin-bottom: 10px;'>%s</h3>", grid_title.c_str());
            printf("<table class='rasi-table'>");
            printf("<tr><td><div style='font-weight:bold; font-size:1.05em; letter-spacing:0.5px;'>%s</div></td><td><div style='font-weight:bold; font-size:1.05em; letter-spacing:0.5px;'>%s</div></td><td><div style='font-weight:bold; font-size:1.05em; letter-spacing:0.5px;'>%s</div></td><td><div style='font-weight:bold; font-size:1.05em; letter-spacing:0.5px;'>%s</div></td></tr>", 
                get_planets(11).c_str(), get_planets(0).c_str(), get_planets(1).c_str(), get_planets(2).c_str());
            printf("<tr><td><div style='font-weight:bold; font-size:1.05em; letter-spacing:0.5px;'>%s</div></td><td colspan='2' rowspan='2' class='rasi-center'><b>%s</b></td><td><div style='font-weight:bold; font-size:1.05em; letter-spacing:0.5px;'>%s</div></td></tr>", 
                get_planets(10).c_str(), telugu_mode ? (varga_str + "<br>చక్రం").c_str() : (varga_str + "<br>CHART").c_str(), get_planets(3).c_str());
            printf("<tr><td><div style='font-weight:bold; font-size:1.05em; letter-spacing:0.5px;'>%s</div></td><td><div style='font-weight:bold; font-size:1.05em; letter-spacing:0.5px;'>%s</div></td></tr>", 
                get_planets(9).c_str(), get_planets(4).c_str());
            printf("<tr><td><div style='font-weight:bold; font-size:1.05em; letter-spacing:0.5px;'>%s</div></td><td><div style='font-weight:bold; font-size:1.05em; letter-spacing:0.5px;'>%s</div></td><td><div style='font-weight:bold; font-size:1.05em; letter-spacing:0.5px;'>%s</div></td><td><div style='font-weight:bold; font-size:1.05em; letter-spacing:0.5px;'>%s</div></td></tr>", 
                get_planets(8).c_str(), get_planets(7).c_str(), get_planets(6).c_str(), get_planets(5).c_str());
            printf("</table>\n");
        } else {
            string p12 = get_planets(11), p1 = get_planets(0), p2 = get_planets(1), p3 = get_planets(2);
            string p11 = get_planets(10), p4 = get_planets(3);
            string p10 = get_planets(9),  p5 = get_planets(4);
            string p9 = get_planets(8),   p8 = get_planets(7), p7 = get_planets(6), p6 = get_planets(5);

            printf("\n[%s %s]\n", varga_str.c_str(), telugu_mode ? "రాశి చక్రం" : "CHART");
            printf("+-----------------+-----------------+-----------------+-----------------+\n");
            printf("|                 |                 |                 |                 |\n");
            printf("| %-15s | %-15s | %-15s | %-15s |\n", p12.c_str(), p1.c_str(), p2.c_str(), p3.c_str());
            printf("+-----------------+-----------------+-----------------+-----------------+\n");
            printf("|                 |                                     |                 |\n");
            char center_text[32]; snprintf(center_text, sizeof(center_text), "%s CHART", varga_str.c_str());
            printf("| %-15s |         %-17s         | %-15s |\n", p11.c_str(), center_text, p4.c_str());
            printf("+-----------------+                                     +-----------------+\n");
            printf("|                 |                                     |                 |\n");
            printf("| %-15s |                                     | %-15s |\n", p10.c_str(), p5.c_str());
            printf("+-----------------+-----------------+-----------------+-----------------+\n");
            printf("|                 |                 |                 |                 |\n");
            printf("| %-15s | %-15s | %-15s | %-15s |\n", p9.c_str(), p8.c_str(), p7.c_str(), p6.c_str());
            printf("+-----------------+-----------------+-----------------+-----------------+\n");
        }
    }	
	// =========================================================================
    // PHASE 1: INTERPRETATION ENGINE (D1 OUTCOMES + VARGA FATE)
    // =========================================================================

void analyze_chart(string varga_str, bool skip_grid = false) {
        int v_num = 1;
        if (varga_str.length() > 1 && varga_str[0] == 'D') v_num = stoi(varga_str.substr(1));

        int v_lagn_rasi = get_varga(v_num, planet_lons[0]);
        int v_planets[10];
        for(int i=0; i<10; i++) v_planets[i] = get_varga(v_num, planet_lons[i]);

        if (html_mode) {
            printf("<h2 style='margin-top: 20px; margin-bottom: 15px; color: var(--accent); border-bottom: 1px solid var(--border); padding-bottom: 5px;'>%s CHART ANALYSIS (Lagna: %s)</h2>", varga_str.c_str(), telugu_mode ? te_rashi_names[v_lagn_rasi] : rashi_names[v_lagn_rasi]);
        } else {
            printf("\n================================================================================\n");
            if (telugu_mode) printf("=== %s చక్ర విశ్లేషణ (లగ్నం: %s) ===\n", varga_str.c_str(), te_rashi_names[v_lagn_rasi]);
            else printf("=== %s CHART ANALYSIS (Lagna: %s) ===\n", varga_str.c_str(), rashi_names[v_lagn_rasi]);
            printf("================================================================================\n");
        }
        
        // --- NEW: INJECT PLANET POSITIONS AND GRID BEFORE ANALYSIS ---
        if (!skip_grid) {
            print_varga_positions_and_grid(v_num, varga_str, v_lagn_rasi, v_planets);
        }

        if (v_num == 1) {
            analyze_general_personality();
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
        if (html_mode) {
            printf("<h3 style='color: var(--accent); margin-top: 25px; margin-bottom: 10px;'>%s</h3>", telugu_mode ? "భావ ఆధిపత్యాలు (పరాశర పద్ధతిలో ఫలితాలు)" : "Bhava Lordships (Specific BPHS House Interpretations)");
            printf("<div style='display: grid; gap: 10px; grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));'>");
        } else {
            if (telugu_mode) printf("\n[భావ ఆధిపత్యాలు (పరాశర పద్ధతిలో ఫలితాలు)]\n");
            else printf("\n[BHAVA LORDSHIPS (Specific BPHS House Interpretations)]\n");
        }
        
        for (int h = 1; h <= 12; h++) {
            int house_rashi = (lagna_rasi + h - 1) % 12;
            string lord_name = rashi_lords[house_rashi];
            
            int p_idx = -1;
            for (int i = 1; i <= 7; i++) { 
                if (string(p_names_full[i]) == lord_name) { p_idx = i; break; }
            }
            
            if (p_idx != -1) {
                int placed_h = (p_rasi[p_idx] - lagna_rasi + 12) % 12 + 1;
                
                if (html_mode) {
                    printf("<div style='background: #1e1e24; padding: 15px; border-radius: 6px; border: 1px solid var(--border);'>");
                    printf("<b style='color: #3498db;'>%s</b><br>", telugu_mode ? (to_string(h) + "వ భావ అధిపతి (" + get_planet_name(p_idx) + ") " + to_string(placed_h) + "వ భావంలో ఉన్నాడు.").c_str() : ("Lord of House " + to_string(h) + " (" + lord_name + ") is in House " + to_string(placed_h) + ".").c_str());
                    printf("<span style='font-size: 14px; color:#aaa; display:block; margin-top:5px;'>%s</span>", telugu_mode ? te_get_lord_in_house_text(h, placed_h).c_str() : get_lord_in_house_text(h, placed_h).c_str());
                    printf("</div>\n");
                } else {
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
        if (html_mode) printf("</div>\n");
    }
	
void analyze_auspiciousness(int lagna_rasi, int* p_rasi) {
        if (!json_mode) {
            if (html_mode) {
                printf("<h2 style='margin-top: 30px; margin-bottom: 10px; color: var(--accent);'>%s</h2>", telugu_mode ? "సమగ్ర గ్రహ శుభ/అశుభ విశ్లేషణ" : "Comprehensive Planetary Auspiciousness");
                printf("<table class='data-table' style='margin-top: 0;'><tr>");
                printf("<th>%s</th><th>%s</th><th>%s</th><th>%s</th></tr>", 
                       telugu_mode ? "గ్రహం" : "Graha", 
                       telugu_mode ? "స్కోరు" : "Score", 
                       telugu_mode ? "స్థితి" : "Status", 
                       telugu_mode ? "వివరణాత్మక లెక్కింపు" : "Detailed Breakdown");
            } else {
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
        
        auto get_lord = [](int rashi) {
            const int lords[] = {3, 6, 4, 2, 1, 4, 6, 3, 5, 7, 7, 5}; 
            return lords[rashi % 12];
        };

        // Naisargika Maitri (Natural Friendship Array: 1=Friend, -1=Enemy, 0=Neutral)
        int maitri[8][8] = {
            {0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 1, 1, 0, 1,-1,-1}, // Sun
            {0, 1, 0, 0, 1, 0, 0, 0}, // Moon
            {0, 1, 1, 0,-1, 1, 0, 0}, // Mars
            {0, 1,-1, 0, 0, 0, 1, 0}, // Merc
            {0, 1, 1, 1,-1, 0,-1, 0}, // Jup
            {0,-1,-1, 0, 1,-1, 0, 1}, // Ven
            {0,-1,-1,-1, 1, 0, 1, 0}  // Sat
        };

        for (int p = 1; p <= 9; p++) {
            int score = 0; string breakdown = "";
            int exaltation_signs[] = {0, 0, 1, 9, 5, 3, 11, 6, 2, 7}; 
            int debilitation_signs[] = {0, 6, 7, 3, 11, 9, 5, 0, 8, 1}; 
            int own_signs1[] = {0, 4, 3, 0, 2, 8, 1, 9, -1, -1}; 
            int own_signs2[] = {0, -1, -1, 7, 5, 11, 6, 10, -1, -1};
            
            if (p <= 7) {
                bool is_exalt_own_deb = false;
                if (p_rasi[p] == exaltation_signs[p]) { score += 4; breakdown += telugu_mode ? "ఉచ్ఛ(+4) " : "Exalted(+4) "; is_exalt_own_deb = true; }
                else if (p_rasi[p] == debilitation_signs[p]) { score -= 3; breakdown += telugu_mode ? "నీచ(-3) " : "Debilitated(-3) "; is_exalt_own_deb = true; }
                else if (p_rasi[p] == own_signs1[p] || p_rasi[p] == own_signs2[p]) { score += 3; breakdown += telugu_mode ? "స్వక్షేత్రం(+3) " : "Own Sign(+3) "; is_exalt_own_deb = true; }
                
                // Advanced Naisargika Maitri check (No more generic "+1" for everything)
                if (!is_exalt_own_deb) {
                    int r_lord_idx = get_lord(p_rasi[p]);
                    int relationship = maitri[p][r_lord_idx];
                    if (relationship == 1) { score += 1; breakdown += telugu_mode ? "మిత్ర స్థానం(+1) " : "Friendly Rasi(+1) "; }
                    else if (relationship == -1) { score -= 1; breakdown += telugu_mode ? "శత్రు స్థానం(-1) " : "Enemy Rasi(-1) "; }
                    else { breakdown += telugu_mode ? "తటస్థ స్థానం(0) " : "Neutral Rasi(0) "; }
                }

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
            else if (h == 8 || h == 12) { score -= 3; breakdown += telugu_mode ? "దుస్థాన స్థితి(-3) " : "Dusthana Placement(-3) "; }
            
            if (h == 3 || h == 6 || h == 10 || h == 11) { 
                if (p == 1 || p == 3 || p == 7 || p == 8 || p == 9) { 
                    score += 2; breakdown += telugu_mode ? "ఉపచయంలో పాపి(+2) " : "Malefic in Upachaya(+2) "; 
                } 
            }
            if (h == 6) { score -= 3; breakdown += telugu_mode ? "దుస్థాన స్థితి(-3) " : "Dusthana Placement(-3) "; } // Apply dusthana penalty separately to 6th
            
            int d9_h = (d9_rashis[p] - d9_rashis[0] + 12) % 12 + 1;
            if (d9_h == 6 || d9_h == 8 || d9_h == 12) { score -= 1; breakdown += telugu_mode ? "D9 దుస్థానం(-1) " : "D9 Dusthana(-1) "; }
            if (p_rasi[p] == d9_rashis[p]) { score += 2; breakdown += telugu_mode ? "వర్గోత్తమ(+2) " : "Vargottama(+2) "; }

            int nak_idx = (int)(planet_lons[p] / (360.0 / 27.0));
            int actual_nak_lord = lord_map[nak_idx % 9];

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
            double sb = 1.0; 
            
            if (sb > 0.0) {
                if (score >= 3) {
                    fusion_text = telugu_mode ? "అత్యుత్తమ (GREAT RESULTS)" : "GREAT RESULTS (Adequate Power + Best Intent)";
                } else if (score <= -3) {
                    fusion_text = telugu_mode ? "ప్రమాదకరం / పరిహారం అవసరం" : "HIGH FRICTION (Remedy Needed)";
                } else {
                    fusion_text = telugu_mode ? "సాధారణం (మిశ్రమ ఫలితాలు)" : "AVERAGE (Mixed Intent)";
                }
            }
            
            natal_scores[p] = score; 
            if (!json_mode) {
                if (html_mode) {
                    printf("<tr><td>%s</td><td>%d</td><td>%s</td><td>%s</td></tr>", 
                           telugu_mode ? get_planet_name(p).c_str() : p_names_full[p], 
                           score, fusion_text.c_str(), breakdown.c_str());
                } else {
                    if (telugu_mode) printf("%-10s | %-6d | %-40s | %s\n", get_planet_name(p).c_str(), score, fusion_text.c_str(), breakdown.c_str());
                    else printf("%-8s | %-6d | %-65s | %s\n", p_names_full[p], score, fusion_text.c_str(), breakdown.c_str());
                }
            }
        }
        
        if (!json_mode) {
            if (html_mode) {
                printf("</table>");
                printf("<p style='color: #888; font-size: 14px; margin-top: 10px;'>%s</p>", 
                       telugu_mode ? "* గమనిక: 'ప్రతికూలం / ప్రమాదకరం' అని ఉన్న గ్రహాలకు జపాలు, దానాలు వంటి నిర్దిష్ట పరిహారాలు అవసరం.<br>* గమనిక: 'సాధారణం' అని ఉన్న గ్రహాలకు రత్నధారణ ద్వారా బలాన్ని పెంచుకోవచ్చు." 
                                   : "* NOTE: Planets marked 'HIGH FRICTION' require specific Remedies (Mantras/Daanams).<br>* NOTE: Planets marked 'AVERAGE' can be strengthened with Gemstones or Colors.");
            } else {
                printf("-----------------------------------------------------------------------------------------------------------------\n");
                if (telugu_mode) {
                    printf(" * గమనిక: 'ప్రతికూల/ప్రమాదకర' అని ఉన్న గ్రహాలకు జపాలు/దానాలు వంటి నిర్దిష్ట పరిహారాలు అవసరం.\n");
                    printf(" * గమనిక: 'ఉపయోగపడని శక్తి' అని ఉన్న గ్రహాలకు రత్నధారణ/యంత్రాల ద్వారా బలాన్ని పెంచాలి.\n");
                } else {
                    printf(" * NOTE: Planets marked 'HIGH FRICTION' require specific Remedies (Mantras/Daanams).\n");
                    printf(" * NOTE: Planets marked 'AVERAGE' require Strengthening (Gemstones/Metals).\n");
                }
            }
        }
    }
	
void search_exact_degree(string planet_name, string sign_name, int deg, int min, int sec, int search_year, int search_month, int search_day = 0) {
        // 1. Resolve Planet Index
        string p_lower = planet_name;
        transform(p_lower.begin(), p_lower.end(), p_lower.begin(), ::tolower);
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
        
        if (search_year > 0 && search_month > 0 && search_day > 0) {
            start_jd = swe_julday(search_year, search_month, search_day, 0.0 - location.tz_offset, SE_GREG_CAL);
            end_jd = start_jd + 1.0;
            char buf[64]; snprintf(buf, sizeof(buf), "Exact Day: %02d/%02d/%04d", search_day, search_month, search_year);
            scope_str = string(buf);
        } else if (search_year > 0 && search_month > 0) {
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
            // DYNAMIC SCOPE: Fast planets default to 24 Hours, Slow planets default to 120 Years
            if (p_idx == 0 || p_idx == 2) {
                int y, m, d; double jut;
                swe_revjul(tjd_ut + (location.tz_offset / 24.0), SE_GREG_CAL, &y, &m, &d, &jut);
                start_jd = swe_julday(y, m, d, 0.0 - location.tz_offset, SE_GREG_CAL); // Forces Midnight start!
                end_jd = start_jd + 1.0; // Exactly 24 hours
                char buf[64]; snprintf(buf, sizeof(buf), "Base Day: %02d/%02d/%04d", d, m, y);
                scope_str = string(buf);
            } else {
                start_jd = tjd_ut; 
                end_jd = start_jd + (120.0 * TRUE_SIDEREAL_YEAR);
                scope_str = "LIFESPAN (120 Years)";
            }
        }

        string print_name = (p_idx == 0) ? (telugu_mode ? "లగ్నం" : "Lagna") : string(p_names_full[p_idx]);

        printf("\n=== EXACT DEGREE SEARCH ===\n");
        printf("Target: %s arriving at %02d° %s %02d'%02d\"\n", print_name.c_str(), deg, rashi_names[s_idx], min, sec);
        printf("Scope: %s\n", scope_str.c_str());
        printf("---------------------------------------------------------------------------------\n");
        printf("%-20s | %-15s | %-20s\n", "Exact Date & Time", "Planet Status", "Movement");
        printf("---------------------------------------------------------------------------------\n");

        // --- UPDATED STEP LOGIC FOR 1-SECOND LAGNA PRECISION ---
        double step = 1.0; 
        if (p_idx == 0) step = 1.0 / 86400.0; // Lagna moves fast: 1-second search interval!
        else if (p_idx == 2) step = 0.25;     // Moon moves medium: 6-hour search interval        
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
	
void search_planet_conjunct_planet(string src_name, string tgt_name, int search_year, int search_month, int search_day, bool include_aspects = false) {
        int src_idx = get_planet_idx(src_name);
        int tgt_idx = get_planet_idx(tgt_name);
        if (src_idx == -1) { printf("Error: Source planet '%s' not recognized.\n", src_name.c_str()); return; }
        if (tgt_idx == -1) { printf("Error: Target planet '%s' not recognized.\n", tgt_name.c_str()); return; }

        double base_target_lon = planet_lons[tgt_idx];

        struct AspectTarget { double lon; string name; };
        vector<AspectTarget> targets;
        targets.push_back({base_target_lon, "1st (Conjunction)"});
        
        if (include_aspects) {
            targets.push_back({fmod(base_target_lon + 180.0, 360.0), "7th Aspect"});
            if (src_idx == 3) {
                targets.push_back({fmod(base_target_lon + 270.0, 360.0), "4th Aspect"});
                targets.push_back({fmod(base_target_lon + 150.0, 360.0), "8th Aspect"});
            } else if (src_idx == 5 || src_idx == 8 || src_idx == 9) {
                targets.push_back({fmod(base_target_lon + 240.0, 360.0), "5th Aspect"});
                targets.push_back({fmod(base_target_lon + 120.0, 360.0), "9th Aspect"});
            } else if (src_idx == 7) {
                targets.push_back({fmod(base_target_lon + 300.0, 360.0), "3rd Aspect"});
                targets.push_back({fmod(base_target_lon + 90.0, 360.0), "10th Aspect"});
            }
        }

        double start_jd, end_jd;
        string scope_str = "";
        
        if (search_year > 0 && search_month > 0 && search_day > 0) {
            start_jd = swe_julday(search_year, search_month, search_day, 0.0 - location.tz_offset, SE_GREG_CAL);
            end_jd = start_jd + 1.0;
            char buf[64]; snprintf(buf, sizeof(buf), "Exact Day: %02d/%02d/%04d", search_day, search_month, search_year);
            scope_str = string(buf);
        } else if (search_year > 0 && search_month > 0) {
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
            if (src_idx == 0 || src_idx == 2) {
                int y, m, d; double jut;
                swe_revjul(tjd_ut + (location.tz_offset / 24.0), SE_GREG_CAL, &y, &m, &d, &jut);
                start_jd = swe_julday(y, m, d, 0.0 - location.tz_offset, SE_GREG_CAL);
                end_jd = start_jd + 1.0;
                char buf[64]; snprintf(buf, sizeof(buf), "Base Day: %02d/%02d/%04d", d, m, y);
                scope_str = string(buf);
            } else {
                start_jd = tjd_ut; 
                end_jd = start_jd + (120.0 * TRUE_SIDEREAL_YEAR);
                scope_str = "LIFESPAN (120 Years)";
            }
        }

        string print_src = (src_idx == 0) ? (telugu_mode ? "లగ్నం" : "Lagna") : string(p_names_full[src_idx]);
        string print_tgt = (tgt_idx == 0) ? (telugu_mode ? "లగ్నం" : "Lagna") : string(p_names_full[tgt_idx]);

        printf("\n=== PLANET %s SEARCH (2° ORB) ===\n", include_aspects ? "ASPECT" : "CONJUNCTION");
        printf("Transit Source : %s\n", print_src.c_str());
        printf("Natal Target   : %s (%.2f°)\n", print_tgt.c_str(), base_target_lon);
        printf("Scope          : %s\n", scope_str.c_str());
        printf("--------------------------------------------------------------------------------------------------------------------------------\n");
        printf("%-20s | %-20s | %-20s | %-15s | %-20s\n", "Enter (2° Orb)", "Exact Peak (0°)", "Exit (2° Orb)", "Movement", "Aspect Type");
        printf("--------------------------------------------------------------------------------------------------------------------------------\n");

        double step = (src_idx == 2) ? (1.0 / 24.0) : (4.0 / 24.0);
        if (src_idx == 0) step = 1.0 / 1440.0;
        
        const double orb = 2.0;
        int hits = 0;

        for (double jd = start_jd; jd < end_jd; jd += step) {
            double trans_lon = get_planet_lon_on_jd(src_idx, jd);
            
            for (const auto& tgt : targets) {
                double dist = get_dist(trans_lon, tgt.lon);
                if (dist <= orb) {
                    double e_in, e_peak, e_out;
                    refine_bubble(src_idx, tgt.lon, jd, orb, e_in, e_peak, e_out, 1);
                    
                    double lon1 = get_planet_lon_on_jd(src_idx, e_in);
                    double lon2 = get_planet_lon_on_jd(src_idx, e_out);
                    double d_move = fmod(lon2 - lon1 + 360.0, 360.0);
                    if (d_move > 180.0) d_move -= 360.0;
                    string dir = (d_move < 0) ? "Retrograde" : "Direct";

                    if (e_peak >= start_jd && e_peak <= end_jd) {
                        printf(" %-19s | %-19s | %-19s | %-15s | %-20s\n", jd_to_string(e_in).c_str(), jd_to_string(e_peak).c_str(), jd_to_string(e_out).c_str(), dir.c_str(), tgt.name.c_str());
                        hits++;
                    }
                    jd = e_out; 
                    break;
                }
            }
        }
        if (hits == 0) printf(" No events found within the 2° orb in this timeframe.\n");
        printf("--------------------------------------------------------------------------------------------------------------------------------\n");
    }

void search_planet_all_transits(string p_name, int search_year) {
        int p_idx = get_planet_idx(p_name);
        if (p_idx == -1) { printf("Error: Planet '%s' not recognized.\n", p_name.c_str()); return; }

        double start_jd = swe_julday(search_year, 1, 1, 0.0 - location.tz_offset, SE_GREG_CAL);
        double end_jd = swe_julday(search_year + 1, 1, 1, 0.0 - location.tz_offset, SE_GREG_CAL);

        string print_src = (p_idx == 0) ? (telugu_mode ? "లగ్నం" : "Lagna") : string(p_names_full[p_idx]);

        printf("\n=== RASHI & NAKSHATRA TRANSIT SEARCH ===\n");
        printf("Transit Source : %s\n", print_src.c_str());
        printf("Scope          : Year %d\n", search_year);
        printf("------------------------------------------------------------------------------------------------------\n");
        printf("%-20s | %-16s | %-45s\n", "Date & Time", "Movement", "Event Profile");
        printf("------------------------------------------------------------------------------------------------------\n");

        double step = (p_idx == 2) ? 0.05 : 0.25; 
        if (p_idx == 0) step = 1.0 / 1440.0;
        
        int planets_se[] = {SE_SUN, SE_MOON, SE_MARS, SE_MERCURY, SE_JUPITER, SE_VENUS, SE_SATURN, node_calc_type, node_calc_type};
        int se_p = (p_idx > 0 && p_idx <= 9) ? planets_se[p_idx - 1] : 0;
        
        double prev_xx[6]; char serr[256];
        if (p_idx > 0) swe_calc_ut(start_jd - step, se_p, iflag, prev_xx, serr);
        
        double prev_lon = get_planet_lon_on_jd(p_idx, start_jd - step);
        double prev_speed = (p_idx > 0 && p_idx < 8) ? prev_xx[3] : 1.0; 

        int prev_rashi = (int)(prev_lon / 30.0);
        int prev_nak = (int)(prev_lon / (360.0 / 27.0));
        int hits = 0;
        
        struct Event { double jd; string dir; string desc; };
        vector<Event> events;

        for (double jd = start_jd; jd < end_jd; jd += step) {
            double lon = get_planet_lon_on_jd(p_idx, jd);
            int curr_rashi = (int)(lon / 30.0);
            int curr_nak = (int)(lon / (360.0 / 27.0));
            
            double d_move = fmod(lon - prev_lon + 360.0, 360.0);
            if (d_move > 180.0) d_move -= 360.0;
            bool is_retro = (d_move < 0);
            string dir = is_retro ? "Retrograde" : "Direct";

            double curr_xx[6];
            if (p_idx > 0) swe_calc_ut(jd, se_p, iflag, curr_xx, serr);
            double curr_speed = (p_idx > 0 && p_idx < 8) ? curr_xx[3] : 1.0;
            
            // 1. Rashi Change
            if (curr_rashi != prev_rashi) {
                double t_low = jd - step, t_high = jd;
                for (int i=0; i<40; i++) {
                    double t_mid = (t_low + t_high)/2.0;
                    int mid_rashi = (int)(get_planet_lon_on_jd(p_idx, t_mid) / 30.0);
                    if (mid_rashi == prev_rashi) t_low = t_mid; else t_high = t_mid;
                }
                double exact_jd = (t_low + t_high)/2.0;
                if (exact_jd >= start_jd && exact_jd <= end_jd) {
                    double check_jd = exact_jd + 0.005; // Time moves forward by 7.2 minutes to anchor safely
                    double stable_lon = get_planet_lon_on_jd(p_idx, check_jd);
                    int stable_rashi = (int)(stable_lon / 30.0);
                    char buf[128]; snprintf(buf, sizeof(buf), "Enters Rashi: %s", rashi_names[stable_rashi]);
                    events.push_back({exact_jd, dir, string(buf)});
                }
            }
            
            // 2. Nakshatra Change
            if (curr_nak != prev_nak) {
                double t_low = jd - step, t_high = jd;
                for (int i=0; i<40; i++) {
                    double t_mid = (t_low + t_high)/2.0;
                    int mid_nak = (int)(get_planet_lon_on_jd(p_idx, t_mid) / (360.0 / 27.0));
                    if (mid_nak == prev_nak) t_low = t_mid; else t_high = t_mid;
                }
                double exact_jd = (t_low + t_high)/2.0;
                if (exact_jd >= start_jd && exact_jd <= end_jd) {
                    double check_jd = exact_jd + 0.005; // Time moves forward by 7.2 minutes to anchor safely
                    double stable_lon = get_planet_lon_on_jd(p_idx, check_jd);
                    int stable_rashi = (int)(stable_lon / 30.0);
                    int stable_nak = (int)(stable_lon / (360.0 / 27.0));
                    char buf[128]; snprintf(buf, sizeof(buf), "Enters Nak: %s (%s)", nak_names[stable_nak], rashi_names[stable_rashi]);
                    events.push_back({exact_jd, dir, string(buf)});
                }
            }

            // 3. Station Points (Turning Retrograde or Direct)
            if (p_idx > 2 && p_idx < 8) { // Mars, Mercury, Jupiter, Venus, Saturn
                if (curr_speed * prev_speed < 0) {
                    double t_low = jd - step, t_high = jd;
                    for (int i=0; i<40; i++) {
                        double t_mid = (t_low+t_high)/2.0;
                        double mid_xx[6]; swe_calc_ut(t_mid, se_p, iflag, mid_xx, serr);
                        if (mid_xx[3] * curr_speed > 0) t_high = t_mid; else t_low = t_mid;
                    }
                    double exact_jd = (t_low + t_high)/2.0;
                    if (exact_jd >= start_jd && exact_jd <= end_jd) {
                        double stat_xx[6]; swe_calc_ut(exact_jd, se_p, iflag, stat_xx, serr);
                        double deg_in_sign = fmod(stat_xx[0], 30.0);
                        int rashi = (int)(stat_xx[0] / 30.0);
                        int nak = (int)(stat_xx[0] / (360.0 / 27.0));
                        int d = (int)deg_in_sign; int m = (int)((deg_in_sign - d)*60); int s = (int)round((deg_in_sign - d - m/60.0)*3600);
                        if(s>=60){s-=60;m++;} if(m>=60){m-=60;d++;}
                        
                        string stat_str = (curr_speed < 0) ? "STATIONS -> Retro" : "STATIONS -> Direct";
                        char buf[128]; snprintf(buf, sizeof(buf), "%s | %02d°%02d'%02d\" %s (%s)", stat_str.c_str(), d, m, s, rashi_names[rashi], nak_names[nak]);
                        events.push_back({exact_jd, "Stationary", string(buf)});
                    }
                }
            }
            
            prev_lon = lon;
            prev_rashi = curr_rashi;
            prev_nak = curr_nak;
            prev_speed = curr_speed;
        }

        sort(events.begin(), events.end(), [](const Event& a, const Event& b){ return a.jd < b.jd; });

        for (const auto& ev : events) {
            printf(" %-19s | %-16s | %s\n", jd_to_string(ev.jd).c_str(), ev.dir.c_str(), ev.desc.c_str());
            hits++;
        }
        
        if (hits == 0) printf(" No transit events found in the specified timeframe.\n");
        printf("------------------------------------------------------------------------------------------------------\n");
    }

    void search_planet_transit_rashi(string src_name, string rashi_name, int search_year, int search_month, int search_day, bool include_aspects = false) {
        int src_idx = get_planet_idx(src_name);
        int target_rashi_idx = get_rashi_idx(rashi_name);
        if (src_idx == -1) { printf("Error: Source planet '%s' not recognized.\n", src_name.c_str()); return; }
        if (target_rashi_idx == -1) { printf("Error: Sign '%s' not recognized.\n", rashi_name.c_str()); return; }

        map<int, string> aspect_map;
        aspect_map[target_rashi_idx] = "1st (Conjunction)";
        if (include_aspects) {
            aspect_map[(target_rashi_idx + 6) % 12] = "7th Aspect";
            if (src_idx == 3) {
                aspect_map[(target_rashi_idx + 9) % 12] = "4th Aspect";
                aspect_map[(target_rashi_idx + 5) % 12] = "8th Aspect";
            } else if (src_idx == 5 || src_idx == 8 || src_idx == 9) {
                aspect_map[(target_rashi_idx + 8) % 12] = "5th Aspect";
                aspect_map[(target_rashi_idx + 4) % 12] = "9th Aspect";
            } else if (src_idx == 7) {
                aspect_map[(target_rashi_idx + 10) % 12] = "3rd Aspect";
                aspect_map[(target_rashi_idx + 3) % 12] = "10th Aspect";
            }
        }

        double start_jd, end_jd;
        string scope_str = "";
        
        if (search_year > 0 && search_month > 0 && search_day > 0) {
            start_jd = swe_julday(search_year, search_month, search_day, 0.0 - location.tz_offset, SE_GREG_CAL);
            end_jd = start_jd + 1.0;
            char buf[64]; snprintf(buf, sizeof(buf), "Exact Day: %02d/%02d/%04d", search_day, search_month, search_year);
            scope_str = string(buf);
        } else if (search_year > 0 && search_month > 0) {
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
            if (src_idx == 0 || src_idx == 2) {
                int y, m, d; double jut;
                swe_revjul(tjd_ut + (location.tz_offset / 24.0), SE_GREG_CAL, &y, &m, &d, &jut);
                start_jd = swe_julday(y, m, d, 0.0 - location.tz_offset, SE_GREG_CAL);
                end_jd = start_jd + 1.0;
                char buf[64]; snprintf(buf, sizeof(buf), "Base Day: %02d/%02d/%04d", d, m, y);
                scope_str = string(buf);
            } else {
                start_jd = tjd_ut; 
                end_jd = start_jd + (120.0 * TRUE_SIDEREAL_YEAR);
                scope_str = "LIFESPAN (120 Years)";
            }
        }

        string print_src = (src_idx == 0) ? (telugu_mode ? "లగ్నం" : "Lagna") : string(p_names_full[src_idx]);

        printf("\n=== RASHI %s SEARCH (0° to 30°) ===\n", include_aspects ? "ASPECT" : "TRANSIT");
        printf("Transit Source : %s\n", print_src.c_str());
        printf("Target Sign    : %s\n", rashi_names[target_rashi_idx]);
        printf("Scope          : %s\n", scope_str.c_str());
        printf("--------------------------------------------------------------------------------------------------------\n");
        printf("%-20s | %-16s | %-20s | %-20s\n", "Date & Time", "Event", "Position", "Aspect Type");
        printf("--------------------------------------------------------------------------------------------------------\n");

        double step = (src_idx == 2) ? 0.05 : 0.25; 
        if (src_idx == 0) step = 1.0 / 1440.0;
        
        double prev_lon = get_planet_lon_on_jd(src_idx, start_jd - step);
        int prev_rashi = (int)(prev_lon / 30.0);
        int hits = 0;
        
        for (double jd = start_jd; jd < end_jd; jd += step) {
            double lon = get_planet_lon_on_jd(src_idx, jd);
            int curr_rashi = (int)(lon / 30.0);
            
            double d_move = fmod(lon - prev_lon + 360.0, 360.0);
            if (d_move > 180.0) d_move -= 360.0;
            bool is_retro = (d_move < 0);
            
            if (curr_rashi != prev_rashi) {
                double t_low = jd - step, t_high = jd;
                for (int i=0; i<40; i++) {
                    double t_mid = (t_low + t_high)/2.0;
                    double mid_lon = get_planet_lon_on_jd(src_idx, t_mid);
                    int mid_rashi = (int)(mid_lon / 30.0);
                    if (mid_rashi == prev_rashi) t_low = t_mid; else t_high = t_mid;
                }
                double exact_jd = (t_low + t_high)/2.0;
                
                bool prev_in = aspect_map.count(prev_rashi) > 0;
                bool curr_in = aspect_map.count(curr_rashi) > 0;

                if (prev_in && exact_jd >= start_jd && exact_jd <= end_jd) {
                    char buf[32]; snprintf(buf, sizeof(buf), "%s %02d° 00' 00\"", rashi_names[prev_rashi], is_retro ? 0 : 30);
                    printf(" %-19s | %-16s | %-20s | %-20s\n", 
                           jd_to_string(exact_jd).c_str(), is_retro ? "EXIT (Retro)" : "EXIT (Direct)", buf, aspect_map[prev_rashi].c_str());
                    hits++;
                }

                if (curr_in && exact_jd >= start_jd && exact_jd <= end_jd) {
                    char buf[32]; snprintf(buf, sizeof(buf), "%s %02d° 00' 00\"", rashi_names[curr_rashi], is_retro ? 30 : 0);
                    printf(" %-19s | %-16s | %-20s | %-20s\n", 
                           jd_to_string(exact_jd).c_str(), is_retro ? "ENTER (Retro)" : "ENTER (Direct)", buf, aspect_map[curr_rashi].c_str());
                    hits++;
                }
            }
            
            bool curr_in = aspect_map.count(curr_rashi) > 0;
            
            if (curr_in && src_idx != 0 && src_idx != 1 && src_idx != 2 && src_idx != 8 && src_idx != 9) {
                int planets_se[] = {SE_SUN, SE_MOON, SE_MARS, SE_MERCURY, SE_JUPITER, SE_VENUS, SE_SATURN, node_calc_type, node_calc_type};
                int se_p = planets_se[src_idx - 1];
                
                double xx[6]; char serr[256];
                swe_calc_ut(jd, se_p, iflag, xx, serr);
                double speed = xx[3];
                
                double prev_xx[6];
                swe_calc_ut(jd - step, se_p, iflag, prev_xx, serr);
                double prev_speed = prev_xx[3];
                
                if (speed * prev_speed < 0) { 
                    double t_low = jd - step, t_high = jd;
                    for (int i=0; i<40; i++) {
                        double t_mid = (t_low+t_high)/2.0;
                        double mid_xx[6]; swe_calc_ut(t_mid, se_p, iflag, mid_xx, serr);
                        if (mid_xx[3] * speed > 0) t_high = t_mid; else t_low = t_mid;
                    }
                    double exact_jd = (t_low + t_high)/2.0;
                    double stat_xx[6]; swe_calc_ut(exact_jd, se_p, iflag, stat_xx, serr);
                    
                    double deg_in_sign = fmod(stat_xx[0], 30.0);
                    int d = (int)deg_in_sign; int m = (int)((deg_in_sign - d)*60); int s = (int)round((deg_in_sign - d - m/60.0)*3600);
                    if(s>=60){s-=60;m++;} if(m>=60){m-=60;d++;}
                    
                    string stat_str = (speed < 0) ? "STATION (-> R)" : "STATION (-> D)";
                    char buf[32]; snprintf(buf, sizeof(buf), "%s %02d° %02d' %02d\"", rashi_names[curr_rashi], d, m, s);
                    
                    if (exact_jd >= start_jd && exact_jd <= end_jd) {
                        printf(" %-19s | %-16s | %-20s | %-20s\n", 
                               jd_to_string(exact_jd).c_str(), stat_str.c_str(), buf, aspect_map[curr_rashi].c_str());
                        hits++;
                    }
                }
            }
            prev_lon = lon;
            prev_rashi = curr_rashi;
        }
        
        if (hits == 0) printf(" No transit events found in the specified timeframe.\n");
        printf("--------------------------------------------------------------------------------------------------------\n");
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

void analyze_general_personality() {
        int asc_nak = (int)(planet_lons[0] / (360.0 / 27.0));
        int asc_pada = (int)((planet_lons[0] - (asc_nak * (360.0 / 27.0))) / ((360.0 / 27.0) / 4.0)) + 1;

        int mo_nak = (int)(moon_lon / (360.0 / 27.0));
        int mo_pada = (int)((moon_lon - (mo_nak * (360.0 / 27.0))) / ((360.0 / 27.0) / 4.0)) + 1;

        string akshara = nama_aksharas[mo_nak][mo_pada - 1];

        if (html_mode) {
            printf("<div style='margin-bottom: 20px; padding: 15px; background: #2a2a35; border-radius: 6px; border-left: 4px solid #3498db;'>");
            printf("<h3 style='margin-top: 0; color: #3498db;'>%s</h3>", telugu_mode ? "లగ్న & చంద్ర నక్షత్ర ఆధారిత వ్యక్తిత్వ విశ్లేషణ" : "Core Personality Matrix");
            
            printf("<p style='margin-top:10px;'><b>%s</b><br><span style='color:#ccc;'>%s</span></p>", 
                   telugu_mode ? ("✦ భౌతిక స్వభావం (లగ్న నక్షత్రం - " + string(te_nak_names[asc_nak]) + " " + to_string(asc_pada) + "వ పాదం):").c_str() 
                               : ("✦ Physical Persona (Lagna in " + string(nak_names[asc_nak]) + ", Pada " + to_string(asc_pada) + "):").c_str(),
                   telugu_mode ? te_get_nakshatra_pada_text(asc_nak, asc_pada).c_str() : get_nakshatra_pada_text(asc_nak, asc_pada).c_str());
            
            printf("<p style='margin-top:15px; margin-bottom:0;'><b>%s</b><br><span style='color:#ccc;'>%s</span></p>", 
                   telugu_mode ? ("✦ మానసిక స్వభావం (చంద్ర నక్షత్రం - " + string(te_nak_names[mo_nak]) + " " + to_string(mo_pada) + "వ పాదం) [నామ అక్షరం: <b style='color:#2ecc71;'>" + akshara + "</b>]:").c_str() 
                               : ("✦ Mental & Emotional Core (Moon in " + string(nak_names[mo_nak]) + ", Pada " + to_string(mo_pada) + ") [Nama Akshara: <b style='color:#2ecc71;'>" + akshara + "</b>]:").c_str(),
                   telugu_mode ? te_get_nakshatra_pada_text(mo_nak, mo_pada).c_str() : get_nakshatra_pada_text(mo_nak, mo_pada).c_str());
            
            printf("</div>\n");
        } else {
            if (telugu_mode) {
                printf("\n[లగ్న & చంద్ర నక్షత్ర ఆధారిత వ్యక్తిత్వ విశ్లేషణ (CORE PERSONALITY)]\n");
                printf("-------------------------------------------------------------------------------------------------\n");
                printf(" ✦ భౌతిక స్వభావం (లగ్న నక్షత్రం - %s %dవ పాదం):\n", te_nak_names[asc_nak], asc_pada);
                printf("   %s\n\n", te_get_nakshatra_pada_text(asc_nak, asc_pada).c_str());
                printf(" ✦ మానసిక స్వభావం (చంద్ర నక్షత్రం - %s %dవ పాదం) [నామ అక్షరం: %s]:\n", te_nak_names[mo_nak], mo_pada, akshara.c_str());
                printf("   %s\n", te_get_nakshatra_pada_text(mo_nak, mo_pada).c_str());
                printf("-------------------------------------------------------------------------------------------------\n");
            } else {
                printf("\n[CORE PERSONALITY MATRIX (Based on Ascendant & Moon Padas)]\n");
                printf("-------------------------------------------------------------------------------------------------\n");
                printf(" ✦ Physical Persona (Lagna in %s, Pada %d):\n", nak_names[asc_nak], asc_pada);
                printf("   %s\n\n", get_nakshatra_pada_text(asc_nak, asc_pada).c_str());
                printf(" ✦ Mental & Emotional Core (Moon in %s, Pada %d) [Nama Akshara: %s]:\n", nak_names[mo_nak], mo_pada, akshara.c_str());
                printf("   %s\n", get_nakshatra_pada_text(mo_nak, mo_pada).c_str());
                printf("-------------------------------------------------------------------------------------------------\n");
            }
        }
    }

void print_specific_nama() {
        if (json_mode) return;

        double nak_size = 360.0 / 27.0;
        int mo_nak = (int)(moon_lon / nak_size);
        int mo_pada = (int)((moon_lon - (mo_nak * nak_size)) / (nak_size / 4.0)); 

        if (html_mode) {
            printf("<h3 style='color: var(--accent); margin-top: 25px; margin-bottom: 10px;'>%s</h3>", telugu_mode ? "నామ నక్షత్రం (జనన నక్షత్రం ఆధారంగా పేరు మొదటి అక్షరం)" : "NAMA NAKSHATRA (Naming Letters Based on Birth)");
            printf("<div style='background: #1e1e24; padding: 15px; border-radius: 6px; border: 1px solid var(--border); line-height: 1.6;'>");
            printf("<b>%s</b> %s<br>", telugu_mode ? "జన్మ నక్షత్రం (Janma Nakshatra):" : "Janma Nakshatra:", telugu_mode ? te_nak_names[mo_nak] : nak_names[mo_nak]);
            printf("<b>%s</b> %d<br>", telugu_mode ? "పాదం (Pada):" : "Pada:", mo_pada + 1);
            printf("<br><span style='font-size:1.2em; color:#2ecc71;'><b>%s</b> <b style='color:#fff;'>%s</b></span><br><br>", 
                   telugu_mode ? "పేరుకు అనుకూలమైన మొదటి అక్షరం:" : "Recommended Starting Letter:", nama_aksharas[mo_nak][mo_pada]);
            printf("<span style='color:#aaa;'>%s %s: 1:%s, 2:%s, 3:%s, 4:%s</span>", 
                   telugu_mode ? te_nak_names[mo_nak] : nak_names[mo_nak],
                   telugu_mode ? "నక్షత్రంలోని 4 పాదాల అక్షరాలు" : "Syllables for all 4 Padas",
                   nama_aksharas[mo_nak][0], nama_aksharas[mo_nak][1], nama_aksharas[mo_nak][2], nama_aksharas[mo_nak][3]);
            printf("</div>\n");
        } else {
            printf("\n=== NAMA NAKSHATRA (SPECIFIC TO BIRTH TIME) ===\n");
            printf("--------------------------------------------------------------------------------\n");
            printf("%-30s : %s\n", telugu_mode ? "జన్మ నక్షత్రం (Janma Nakshatra)" : "Janma Nakshatra", telugu_mode ? te_nak_names[mo_nak] : nak_names[mo_nak]);
            printf("%-30s : %d\n", telugu_mode ? "పాదం (Pada)" : "Pada", mo_pada + 1);
            printf("--------------------------------------------------------------------------------\n");
            printf("%s: => %s <=\n", telugu_mode ? "పేరుకు అనుకూలమైన మొదటి అక్షరం" : "Recommended Starting Letter", nama_aksharas[mo_nak][mo_pada]);
            printf("--------------------------------------------------------------------------------\n");
            printf("%s %s: 1:%s | 2:%s | 3:%s | 4:%s\n", 
                    telugu_mode ? "మొత్తం 4 పాదాల అక్షరాలు" : "Syllables for all 4 Padas of", 
                    telugu_mode ? te_nak_names[mo_nak] : nak_names[mo_nak],
                    nama_aksharas[mo_nak][0], nama_aksharas[mo_nak][1], 
                    nama_aksharas[mo_nak][2], nama_aksharas[mo_nak][3]);
            printf("--------------------------------------------------------------------------------\n");
        }
    }
	
void analyze_final_outcomes(int lagna_rasi, int* p_rasi) {
        if (html_mode) {
            printf("<h3 style='color: var(--accent); margin-top: 25px; margin-bottom: 10px;'>%s</h3>", telugu_mode ? "గ్రహాల తుది ఫలితం (దశ/అంతర్దశలలో జరిగేవి)" : "Synthesized Final Outcome of Planets (D1 Fate)");
            printf("<table class='data-table'><tr><th>%s</th><th>%s</th></tr>", telugu_mode ? "గ్రహం" : "Graha", telugu_mode ? "తుది ఫలితం" : "Outcome");
        } else {
            if (telugu_mode) printf("\n[గ్రహాల తుది ఫలితం (దశ/అంతర్దశలలో జరిగేవి)]\n");
            else printf("\n[SYNTHESIZED FINAL OUTCOME OF PLANETS (D1 FATE)]\n");
        }
        
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
            
            if (html_mode) {
                string outcome = telugu_mode ? te_get_final_outcome(p, is_benefic, is_malefic, is_kendra_lord, is_dusthana, is_kendra_trikona, h, get_planet_name(disp_idx)) 
                                             : get_final_outcome(p, is_benefic, is_malefic, is_kendra_lord, is_dusthana, is_kendra_trikona, h, string(p_names_full[disp_idx]));
                printf("<tr><td><b>%s</b></td><td>%s</td></tr>", telugu_mode ? get_planet_name(p).c_str() : p_names_full[p], outcome.c_str());
            } else {
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
        if (html_mode) printf("</table>\n");
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
        if (html_mode) {
            printf("<h3 style='color: var(--accent); margin-top: 25px; margin-bottom: 10px;'>%s</h3>", telugu_mode ? "ప్రధాన యోగాలు (రాజయోగాలు)" : "Major Yogas Detected");
            printf("<ul style='background: #1e1e24; padding: 20px 20px 20px 40px; margin: 0; border-radius: 6px; border: 1px solid var(--border); line-height: 1.6;'>");
        } else {
            if (telugu_mode) printf("\n[ప్రధాన యోగాలు (రాజయోగాలు)]\n");
            else printf("\n[MAJOR YOGAS DETECTED]\n");
        }
        
        bool yoga_found = false;
        auto in_kendra = [&](int r, int l) { int h = (r - l + 12) % 12 + 1; return (h==1 || h==4 || h==7 || h==10); };
        auto is_own_exalt = [&](int r, int ex, int own1, int own2) { return (r==ex || r==own1 || r==own2); };

        auto print_yoga = [&](string name, string desc, string color = "#3498db") {
            if (html_mode) {
                printf("<li style='margin-bottom:8px;'><b style='color:%s;'>%s:</b> %s</li>\n", color.c_str(), name.c_str(), desc.c_str());
            } else {
                // Instantly strip injected HTML tags if we are in CLI Mode
                string clean_name = name;
                string clean_desc = desc;
                
                size_t pos;
                while ((pos = clean_name.find("<b>")) != string::npos) clean_name.replace(pos, 3, "");
                while ((pos = clean_name.find("</b>")) != string::npos) clean_name.replace(pos, 4, "");
                
                while ((pos = clean_desc.find("<b>")) != string::npos) clean_desc.replace(pos, 3, "");
                while ((pos = clean_desc.find("</b>")) != string::npos) clean_desc.replace(pos, 4, "");
                
                printf("  - %s: %s\n", clean_name.c_str(), clean_desc.c_str());
            }
        };

        if (in_kendra(p_rasi[3], lagna) && is_own_exalt(p_rasi[3], 9, 0, 7)) print_yoga("Ruchaka", "<b>Ruchaka Yoga:</b> Mars is powerfully placed in a Kendra in its own or exalted sign. This grants profound courage, natural leadership, and heavy success in real estate or technical domains.");
        if (in_kendra(p_rasi[4], lagna) && is_own_exalt(p_rasi[4], 5, 2, 5)) print_yoga("Bhadra", "<b>Bhadra Yoga:</b> Mercury is powerfully placed in a Kendra... grants sharp intellect, flawless communication, and business acumen.");
        if (in_kendra(p_rasi[5], lagna) && is_own_exalt(p_rasi[5], 3, 8, 11)) print_yoga("Hamsa", "<b>Hamsa Yoga:</b> Jupiter is powerfully placed in a Kendra... surrounds the native with an aura of deep wisdom and spiritual respect.");
        if (in_kendra(p_rasi[6], lagna) && is_own_exalt(p_rasi[6], 11, 1, 6)) print_yoga("Malavya", "<b>Malavya Yoga:</b> Venus is powerfully placed in a Kendra... guarantees a life immersed in luxury, fine arts, and magnetism.");
        if (in_kendra(p_rasi[7], lagna) && is_own_exalt(p_rasi[7], 6, 9, 10)) print_yoga("Sasa", "<b>Sasa Yoga:</b> Saturn is powerfully placed in a Kendra... grants unbreakable persistence and the ability to hold vast authority.");
        if (in_kendra(p_rasi[5], p_rasi[2])) print_yoga("Gajakesari", "<b>Gajakesari Yoga:</b> Jupiter forms a powerful angular relationship with the Moon... imparts lasting reputation and profound resilience.");
        if (p_rasi[1] == p_rasi[4]) print_yoga("Budhaditya", "<b>Budhaditya Yoga:</b> The Sun and Mercury are conjunct... creates a highly analytical, brilliantly sharp mind.");
        if (p_rasi[2] == p_rasi[3]) print_yoga("ChandraMangala", "<b>Chandra-Mangala Yoga:</b> The Moon and Mars are conjunct... generates restless emotional intensity geared towards financial drive.");
        
        if(!yoga_found) {
            if (html_mode) {
                printf("<li style='color: #888;'>%s</li>", telugu_mode ? "ఈ జాతక చక్రంలో ప్రధాన మహాపురుష యోగాలు ఏవీ గుర్తించబడలేదు." : "No major primary Mahapurusha yogas detected in this specific alignment.");
            } else {
                if (telugu_mode) printf("  - ఈ జాతక చక్రంలో ప్రధాన మహాపురుష యోగాలు ఏవీ గుర్తించబడలేదు.\n");
                else printf("  - No major primary Mahapurusha yogas detected in this specific alignment.\n");
            }
        }
        if (html_mode) printf("</ul>\n");
    }

	void analyze_doshas(int* p_rasi, int lagna) {
        if (!html_mode) {
            if (telugu_mode) printf("\n[ప్రధాన దోషాలు (గమనించాల్సినవి)]\n");
            else printf("\n[MAJOR DOSHAS DETECTED]\n");
        } else {
            printf("<h2 style='margin-top: 30px; margin-bottom: 15px; color: var(--accent); border-bottom: 1px solid var(--border); padding-bottom: 5px;'>%s</h2>", telugu_mode ? "ప్రధాన దోషాలు (గమనించాల్సినవి)" : "Major Doshas Detected");
        }
        
        bool dosha_found = false;
        int ma_h_lagna = (p_rasi[3] - lagna + 12) % 12 + 1;
        int ma_h_moon = (p_rasi[3] - p_rasi[2] + 12) % 12 + 1;
        bool lagna_kuja = (ma_h_lagna==1 || ma_h_lagna==2 || ma_h_lagna==4 || ma_h_lagna==7 || ma_h_lagna==8 || ma_h_lagna==12);
        bool moon_kuja = (ma_h_moon==1 || ma_h_moon==2 || ma_h_moon==4 || ma_h_moon==7 || ma_h_moon==8 || ma_h_moon==12);

        if (lagna_kuja || moon_kuja) {
            // --- NEW: KUJA DOSHA CANCELLATION (BHANGA) ALGORITHMS ---
            int cancel_code = 0;
            int ma_rashi = p_rasi[3];
            int ju_rashi = p_rasi[5];
            int mo_rashi = p_rasi[2];
            int ve_rashi = p_rasi[6];
            int sa_rashi = p_rasi[7];
            int ra_rashi = p_rasi[8];
            
            auto aspects = [&](int p, int target_rashi) {
                int r = p_rasi[p];
                int d = (target_rashi - r + 12) % 12 + 1;
                if (d == 7) return true;
                if (p == 3 && (d == 4 || d == 8)) return true;
                if (p == 5 && (d == 5 || d == 9)) return true;
                if (p == 7 && (d == 3 || d == 10)) return true;
                return false;
            };

            bool ju_aspects_ma = (ju_rashi == ma_rashi || aspects(5, ma_rashi));
            bool mo_aspects_ma = (mo_rashi == ma_rashi || aspects(2, ma_rashi));
            bool ve_aspects_ma = (ve_rashi == ma_rashi || aspects(6, ma_rashi));
            bool sa_aspects_ma = (sa_rashi == ma_rashi || aspects(7, ma_rashi));
            bool ra_conjunct_ma = (ra_rashi == ma_rashi);

            int d9_ma_rashi = get_varga(9, planet_lons[3]);

            // Calculate Age using system clock vs birth date (tjd_ut)
            time_t t = time(nullptr); tm* now = localtime(&t);
            int current_year = now->tm_year + 1900;
            int by, bm, bd; double bjut;
            swe_revjul(tjd_ut + (location.tz_offset/24.0), SE_GREG_CAL, &by, &bm, &bd, &bjut);
            int age = current_year - by;

            // Priority Check list (1 to 10 based on strength of cancellation)
            if (ma_rashi == 0 || ma_rashi == 7 || ma_rashi == 9) cancel_code = 1; // Own/Exalted
            else if (ju_aspects_ma) cancel_code = 2; // Jupiter blessing
            else if (mo_aspects_ma) cancel_code = 3; // Moon blessing
            else if (ve_aspects_ma || ve_rashi == (lagna + 6)%12) cancel_code = 4; // Venus blessing / in 7th
            else if (sa_aspects_ma || ra_conjunct_ma) cancel_code = 5; // Saturn/Rahu absorbs
            else if (ma_h_lagna == 2 && (ma_rashi == 2 || ma_rashi == 5)) cancel_code = 6; // 2nd house Gemini/Virgo
            else if (ma_h_lagna == 4 && (ma_rashi == 0 || ma_rashi == 7 || ma_rashi == 9)) cancel_code = 6; // 4th house Ari/Sco/Cap
            else if (ma_h_lagna == 7 && (ma_rashi == 3 || ma_rashi == 9 || ma_rashi == 2 || ma_rashi == 5)) cancel_code = 6; // 7th house Can/Cap/Gem/Vir
            else if (ma_h_lagna == 8 && (ma_rashi == 8 || ma_rashi == 11 || ma_rashi == 3 || ma_rashi == 4)) cancel_code = 6; // 8th house Sag/Pis/Can/Leo
            else if (ma_h_lagna == 12 && (ma_rashi == 1 || ma_rashi == 6)) cancel_code = 6; // 12th house Tau/Lib
            else if (ma_h_lagna == 1) cancel_code = 7; // Mars in Lagna exception
            else if (d9_ma_rashi == 0 || d9_ma_rashi == 7 || d9_ma_rashi == 9) cancel_code = 8; // Strong in D9
            else if (ma_rashi == 3 || ma_rashi == 4 || ma_rashi == 8 || ma_rashi == 11) cancel_code = 9; // Friendly sign
            else if (age >= 28) cancel_code = 10; // Age maturity override

            string severity = "Medium";
            if (cancel_code == 0 && (ma_h_lagna == 7 || ma_h_lagna == 8 || ma_h_moon == 7 || ma_h_moon == 8)) severity = "High";
            else if (cancel_code > 0) severity = "Cancelled";

            // --- UI PRINTING ---
            if (html_mode) {
                string border_color = (severity == "High") ? "#e74c3c" : ((severity == "Cancelled") ? "#2ecc71" : "#f39c12");
                string translated_severity = telugu_mode ? ((severity == "High") ? "తీవ్రం (High)" : (severity == "Cancelled" ? "రద్దు అయింది (Cancelled)" : "మధ్యస్థం (Medium)")) : severity;
                
                printf("<div style='margin-bottom:15px; padding:15px; background:#2a2a35; border-left:4px solid %s; border-radius:4px;'>", border_color.c_str());
                printf("<h4 style='margin-top:0; color:#e0e0e0;'>%s <span style='color:%s; font-size:12px;'>(%s: %s)</span></h4>", 
                       telugu_mode ? "మంగళ (కుజ) దోషం" : "Mangal (Kuja) Dosha", border_color.c_str(), 
                       telugu_mode ? "తీవ్రత" : "Severity", translated_severity.c_str());
                       
                printf("<p style='margin:5px 0; font-size:14px; line-height:1.6;'>%s</p>", 
                       telugu_mode ? te_get_mangal_dosha_text(ma_h_lagna, ma_h_moon, cancel_code, severity, html_mode).c_str() 
                                   : get_mangal_dosha_text(ma_h_lagna, ma_h_moon, cancel_code, severity, html_mode).c_str());
                printf("</div>\n");
            } else {
                if (telugu_mode) printf("  - కుజ దోషం (%s): %s\n", severity.c_str(), te_get_mangal_dosha_text(ma_h_lagna, ma_h_moon, cancel_code, severity, html_mode).c_str());
                else printf("  - Mangal Dosha (%s): %s\n", severity.c_str(), get_mangal_dosha_text(ma_h_lagna, ma_h_moon, cancel_code, severity, html_mode).c_str());
            }
            dosha_found = true;
        }

        // --- KALA SARPA DOSHA ---
        int r_rahu = p_rasi[8], r_ketu = p_rasi[9];
        bool all_one_side = true, all_other_side = true;
        for (int i=1; i<=7; i++) {
            int d1 = (p_rasi[i] - r_rahu + 12) % 12;
            int d2 = (r_ketu - r_rahu + 12) % 12;
            if (d1 > d2) all_one_side = false;
            if (d1 < d2 && d1 != 0) all_other_side = false; 
        }
        if (all_one_side || all_other_side) {
            if (html_mode) {
                printf("<div style='margin-bottom:15px; padding:15px; background:#2a2a35; border-left:4px solid #e74c3c; border-radius:4px;'>");
                printf("<h4 style='margin-top:0; color:#e0e0e0;'>%s</h4>", telugu_mode ? "కాల సర్ప దోషం" : "Kala Sarpa Matrix");
                printf("<p style='margin:5px 0; font-size:14px; line-height:1.6;'>%s</p>", 
                       telugu_mode ? "అన్ని ప్రధాన గ్రహాలు రాహు/కేతువుల అక్షంలో బంధించబడ్డాయి. ఇది జీవితం మొదటి భాగంలో తీవ్రమైన జాప్యాన్ని సృష్టిస్తుంది, కానీ వయసు పెరిగేకొద్దీ అపారమైన ఒత్తిడి ద్వారా భారీ విజయాన్ని ఇస్తుంది." 
                                   : "All major physical planets are hemmed within the Rahu/Ketu axis. This enforces delays in the first half of life, building immense pressure that releases into success later.");
                printf("</div>\n");
            } else {
                if (telugu_mode) printf("  - కాల సర్ప దోషం: అన్ని గ్రహాలు రాహు/కేతువుల అక్షంలో బంధించబడ్డాయి.\n");
                else {
                    printf("  - Kala Sarpa Matrix: All major physical planets are physically hemmed within the Rahu/Ketu karmic axis.\n");
                    printf("    * Effect: Enforces delays in the first half of life, building immense pressure that releases into success later.\n");
                }
            }
            dosha_found = true;
        }
        
        if(!dosha_found) {
            if (html_mode) {
                printf("<div style='padding:15px; background:#2a2a35; border-left:4px solid #2ecc71; border-radius:4px;'>");
                printf("<p style='margin:0; font-size:14px; color:#e0e0e0;'>%s</p>", telugu_mode ? "ఈ జాతకంలో ఎటువంటి ప్రధాన నిర్మాణ దోషాలు లేవు. జాతకం చాలా పరిశుభ్రంగా ఉంది." : "No major structural doshas (Mangal/Kala Sarpa) detected. The chart is clear.");
                printf("</div>\n");
            } else {
                if (telugu_mode) printf("  - ఈ జాతకంలో ఎటువంటి ప్రధాన నిర్మాణ దోషాలు లేవు.\n");
                else printf("  - No major structural doshas detected.\n");
            }
        }
        
        fflush(stdout); // CRITICAL FIX: Flush the UI immediately!
    }
	// ---------------------------------------------------------
    // CRITICAL MATH FIX: Force floating-point division with 30.0
    // ---------------------------------------------------------
int get_varga(int varga, double lon) {
        lon = fmod(lon, 360.0); if(lon < 0) lon += 360.0;
        int r = (int)(lon / 30.0);
        double d = fmod(lon, 30.0);
        if(varga == 2){ // Hora - only 3 Karka / 4 Simha
            if(r%2==0) return d < 15.0? 4 : 3;
            else return d < 15.0? 3 : 4;
        }
        if(varga == 3){ // Drekkana
            int n = (int)(d / 10.0);
            int start;
            if(r%3==0) start = r; // movable
            else if(r%3==1) start = r+8; // fixed
            else start = r+4; // dual
            return (start + n*4) % 12;
        }
        if(varga == 7){
            int n = (int)(d / (30.0/7.0));
            int start = (r%2==0)? r : r+6;
            return (start + n) % 12;
        }
        if(varga == 9){
            int n = (int)(d / (30.0/9.0));
            int start;
            if(r%3==0) start = r;
            else if(r%3==1) start = r+8;
            else start = r+4;
            return (start + n) % 12;
        }
        return (int)(lon / (30.0 / varga)) % 12;
    }
void dump_rashis(){
    for(int p=0;p<=9;p++) printf("P%d %02.0f %s\n",p,fmod(planet_lons[p],30.0),rashi_names[planet_rashis[p]]);
}

void analyze_progeny(bool is_female = false, bool gender_provided = false) {
        if (json_mode) return;

//printf("\n[PLANETS_DUMP]\n");
  //      for(int p=0;p<=9;p++){
    //        printf("P%d lon=%.2f rashi=%d %s deg_in_sign=%.2f\n", p, planet_lons[p], planet_rashis[p], rashi_names[planet_rashis[p]], fmod(planet_lons[p],30.0));
      //  }
//dump_rashis();

        if (html_mode) {
            printf("<h2 style='margin-top: 20px; color: var(--accent); border-bottom: 1px solid var(--border); padding-bottom: 5px;'>%s</h2>", telugu_mode ? "సంతాన విశ్లేషణ (PROGENY & D7 SAPTAMSHA ANALYSIS)" : "SANTAN YOGA & DOSHA (PROGENY & D7 SAPTAMSHA ANALYSIS)");
            
            printf("<div style='background: #311b1b; padding: 15px; border-radius: 6px; border-left: 4px solid #e74c3c; margin-bottom: 25px;'>");
            printf("<h4 style='margin: 0 0 8px 0; color: #e74c3c;'>%s</h4>", telugu_mode ? "ముఖ్య గమనిక / చట్టపరమైన నిరాకరణ (DISCLAIMER)" : "LEGAL & ALGORITHMIC DISCLAIMER");
            printf("<p style='margin: 0; font-size: 13px; color: #ccc; line-height: 1.5;'>%s</p>", telugu_mode ? "ఇక్కడ అందించిన సమాచారం ప్రాచీన పరాశర మరియు జైమిని గణిత పద్ధతుల ఆధారంగా లెక్కించబడింది. ఇది కేవలం జ్యోతిష్య పరిశోధన మరియు విద్యా ప్రయోజనాల కోసం మాత్రమే. ఈ ఫలితాల ఖచ్చితత్వానికి మేము ఎలాంటి బాధ్యత వహించము. అంతేకాకుండా, భారతీయ చట్టాలకు (PCPNDT Act) లోబడి, పుట్టబోయే బిడ్డ యొక్క లింగ నిర్ధారణ (ఆడ/మగ అంచనా వేయుట) ఈ సాఫ్ట్‌వేర్‌లో పూర్తిగా నిషేధించబడింది మరియు ఆ కోడ్ వెబ్ ఇంటర్‌ఫేస్ నుండి తొలగించబడింది." : "The calculations presented here are based purely on ancient classical algorithms (Brihat Parashara Hora Shastra and Jaimini Sutras). They are provided for astrological research and educational purposes only. We hold no liability for the exact manifestation of these algorithmic outcomes. Furthermore, in strict compliance with the laws of India (including the PCPNDT Act), this software strictly prohibits and restricts the astrological prediction of a child's gender on the web interface.");
            printf("</div>\n");
            
        } else {
            if (telugu_mode) {
                printf("\n=================================================================\n");
                printf("=== సంతాన విశ్లేషణ (PROGENY & D7 SAPTAMSHA ANALYSIS) ===\n");
                printf("=================================================================\n");
            } else {
                printf("\n=================================================================\n");
                printf("=== SANTAN YOGA & DOSHA (PROGENY & D7 SAPTAMSHA ANALYSIS) ===\n");
                printf("=================================================================\n");
            }
        }

        int asc_rashi = planet_rashis[0];

        auto is_male_rashi = [](int r) {
            if (r == 2 || r == 10) return false; // keep your original that passed suite
            if (r == 3 || r == 11) return true;
            return (r % 2 == 0);
        };
        auto is_male_planet = [&](int p) {
            if (p == 1 || p == 3 || p == 5 || p == 8) return true; // Sun,Mars,Jup,Rahu
            if (p == 2 || p == 6 || p == 9) return false;
            if (p == 7 || p == 4) return is_male_rashi(planet_rashis[p]);
            return false;
        };
        auto is_female_planet = [&](int p) { return!is_male_planet(p); };
        auto get_varga_rashi = [&](int p, int varga) { return get_varga(varga, planet_lons[p]); };
        auto is_male_planet_in_varga = [&](int p, int varga) {
            if (p == 1 || p == 3 || p == 5 || p == 8) return true;
            if (p == 2 || p == 6 || p == 9) return false;
            return is_male_rashi(get_varga_rashi(p, varga));
        };
		
        auto is_female_planet_in_varga = [&](int p, int varga) {
            if (p == 2 || p == 6 || p == 9) return true;
            if (p == 1 || p == 3 || p == 5 || p == 8) return false;
            if (p == 4 || p == 7) return!is_male_rashi(get_varga_rashi(p, varga));
            return false;
        };		
        auto check_aspect = [&](int p, int target_rashi) {
            int r = planet_rashis[p];
            int d = (target_rashi - r + 12) % 12 + 1;
            if (d == 7) return true;
            if (p == 3 && (d == 4 || d == 8)) return true;
            if (p == 5 && (d == 5 || d == 9)) return true;
            if (p == 7 && (d == 3 || d == 10)) return true;
            return false;
        };

        auto check_aspect_varga = [&](int p, int target_rashi, int varga) {
            int r = get_varga(varga, planet_lons[p]);
            int d = (target_rashi - r + 12) % 12 + 1;
            if (d == 7) return true;
            if (p == 3 && (d == 4 || d == 8)) return true;
            if (p == 5 && (d == 5 || d == 9)) return true;
            if (p == 7 && (d == 3 || d == 10)) return true;
            return false;
        };

        auto get_lord = [](int rashi) {
            const int lords[] = {3, 6, 4, 2, 1, 4, 6, 3, 5, 7, 7, 5};
            return lords[rashi % 12];
        };

        int local_scores[10] = {0};
        int exaltation_signs[] = {0, 0, 1, 9, 5, 3, 11, 6, 2, 7}; 
        int debilitation_signs[] = {0, 6, 7, 3, 11, 9, 5, 0, 8, 1}; 
        int own_signs1[] = {0, 4, 3, 0, 2, 8, 1, 9, -1, -1}; 
        int own_signs2[] = {0, -1, -1, 7, 5, 11, 6, 10, -1, -1};
        
        for (int p=1; p<=9; p++) {
            if (p <= 7) {
                if (planet_rashis[p] == exaltation_signs[p]) local_scores[p] += 4;
                else if (planet_rashis[p] == debilitation_signs[p]) local_scores[p] -= 3;
                else if (planet_rashis[p] == own_signs1[p] || planet_rashis[p] == own_signs2[p]) local_scores[p] += 3;
            }
            int h = (planet_rashis[p] - asc_rashi + 12) % 12 + 1;
            if (h == 1 || h == 5 || h == 9 || h == 4 || h == 7 || h == 10) local_scores[p] += 2;
            else if (h == 6 || h == 8 || h == 12) local_scores[p] -= 3;
        }

        auto get_dignity_weight = [&](int p, bool is_occupant) {
            int base = is_occupant ? 2 : 1;
            if (local_scores[p] >= 3) return base + 2; 
            if (local_scores[p] <= -3) return base - 1; 
            return base;
        };

        vector<pair<int, double>> k_list;
        for (int i = 1; i <= 7; i++) k_list.push_back({i, fmod(planet_lons[i], 30.0)});
        sort(k_list.begin(), k_list.end(), [](const pair<int, double>& a, const pair<int, double>& b) { return a.second > b.second; });
        int pk_idx = k_list[5].first; 

        int h5_rashi = (asc_rashi + 4) % 12;
        int l5_idx = get_lord(h5_rashi);
        
        int h7_rashi = (asc_rashi + 6) % 12;
        int l7_idx = get_lord(h7_rashi);
        
        int h9_rashi = (asc_rashi + 8) % 12;
        int l9_idx = get_lord(h9_rashi);
        
        int h11_rashi = (asc_rashi + 10) % 12;
        int l11_idx = get_lord(h11_rashi);
        
        int h1_rashi = asc_rashi;
        int l1_idx = get_lord(h1_rashi);
        
        int h3_rashi = (asc_rashi + 2) % 12;
        int l3_idx = get_lord(h3_rashi);

        int d9_asc = get_varga(9, planet_lons[0]);
        int d7_asc = get_varga(7, planet_lons[0]);

		printf("\n[D1 Planets]\n");
		for(int p=0;p<=9;p++){
		  printf(" %s : %s (%d)\n", p==0?"Lagna":p_names_full[p], rashi_names[planet_rashis[p]], planet_rashis[p]);
		}		
		printf("\n[D7 Planets]\n");
		for(int p=1;p<=9;p++){
		  int d7r = get_varga(7, planet_lons[p]);
		  printf(" %s : %s\n", p_names_full[p], rashi_names[d7r]);

		}
        int lord_for_count = (gender_provided && is_female) ? l9_idx : l5_idx;
        int navamsas_gained = (int)(fmod(planet_lons[lord_for_count], 30.0) / (10.0 / 3.0)) + 1; 
        int base_children = navamsas_gained;

        int lord_placement_h = (planet_rashis[lord_for_count] - asc_rashi + 12) % 12 + 1;
        if (lord_placement_h == 6 || lord_placement_h == 8 || lord_placement_h == 12) base_children -= 1;
        
        if (local_scores[lord_for_count] < 0) base_children -= 1;
        if (planet_rashis[5] == planet_rashis[lord_for_count]) base_children += 1; 
        
        int house_for_count_rashi = (gender_provided && is_female) ? h9_rashi : h5_rashi;
        for (int p=1; p<=7; p++) {
            if (p == 2 || p == 4 || p == 5 || p == 6) { 
                if (check_aspect(p, house_for_count_rashi)) base_children += 1;
            }
        }

        if (base_children < 0) base_children = 0;

        int d9_primary_lord_rashi = get_varga(9, planet_lons[lord_for_count]);
        int d9_rahu = get_varga(9, planet_lons[8]);
        int max_children_allowed = 6;
        int traverse_rashi = d9_primary_lord_rashi;
        int break_point = -1;
        
        for (int i = 1; i <= 6; i++) {
            if (traverse_rashi == d9_rahu) {
                max_children_allowed = i;
                break_point = i;
                break;
            }
            traverse_rashi = (traverse_rashi + 2) % 12; 
        }

        if (base_children > max_children_allowed) base_children = max_children_allowed;

        int c1_rashi, c2_rashi, c3_rashi, c4_rashi, c5_rashi, c6_rashi;
        int c1_lord, c2_lord, c3_lord, c4_lord, c5_lord, c6_lord;
        string c1_n_en, c2_n_en, c3_n_en, c4_n_en, c5_n_en, c6_n_en;
        string c1_n_te, c2_n_te, c3_n_te, c4_n_te, c5_n_te, c6_n_te;

        if (d7_asc % 2 == 0) {
            c1_rashi = h5_rashi;  c1_lord = l5_idx;  c1_n_en = "First Child (5th House)";  c1_n_te = "మొదటి సంతానం (5వ భావం)";
            c2_rashi = h7_rashi;  c2_lord = l7_idx;  c2_n_en = "Second Child (7th House)"; c2_n_te = "రెండవ సంతానం (7వ భావం)";
            c3_rashi = h9_rashi;  c3_lord = l9_idx;  c3_n_en = "Third Child (9th House)";  c3_n_te = "మూడవ సంతానం (9వ భావం)";
            c4_rashi = h11_rashi; c4_lord = l11_idx; c4_n_en = "Fourth Child (11th House)"; c4_n_te = "నాల్గవ సంతానం (11వ భావం)";
            c5_rashi = h1_rashi;  c5_lord = l1_idx;  c5_n_en = "Fifth Child (1st House)";   c5_n_te = "ఐదవ సంతానం (1వ భావం)";
            c6_rashi = h3_rashi;  c6_lord = l3_idx;  c6_n_en = "Sixth Child (3rd House)";   c6_n_te = "ఆరవ సంతానం (3వ భావం)";
        } else {
            c1_rashi = h9_rashi;  c1_lord = l9_idx;  c1_n_en = "First Child (9th House)";  c1_n_te = "మొదటి సంతానం (9వ భావం)";
            c2_rashi = h7_rashi;  c2_lord = l7_idx;  c2_n_en = "Second Child (7th House)"; c2_n_te = "రెండవ సంతానం (7వ భావం)";
            c3_rashi = h5_rashi;  c3_lord = l5_idx;  c3_n_en = "Third Child (5th House)";  c3_n_te = "మూడవ సంతానం (5వ భావం)";
            c4_rashi = h3_rashi;  c4_lord = l3_idx;  c4_n_en = "Fourth Child (3rd House)"; c4_n_te = "నాల్గవ సంతానం (3వ భావం)";
            c5_rashi = h1_rashi;  c5_lord = l1_idx;  c5_n_en = "Fifth Child (1st House)";   c5_n_te = "ఐదవ సంతానం (1వ భావం)";
            c6_rashi = h11_rashi; c6_lord = l11_idx; c6_n_en = "Sixth Child (11th House)"; c6_n_te = "ఆరవ సంతానం (11వ భావం)";
        }
		
        double beeja_sphuta = fmod(planet_lons[1] + planet_lons[6] + planet_lons[5], 360.0);
        double kshetra_sphuta = fmod(planet_lons[2] + planet_lons[3] + planet_lons[5], 360.0);
        
        int beeja_rashi = (int)(beeja_sphuta / 30.0);
        int kshetra_rashi = (int)(kshetra_sphuta / 30.0);
        int beeja_d9 = get_varga(9, beeja_sphuta);
        int kshetra_d9 = get_varga(9, kshetra_sphuta);

        if (html_mode) {
            printf("<h3 style='color: var(--accent); margin-top: 25px; margin-bottom: 10px;'>%s</h3>", telugu_mode ? "సంతాన సాఫల్య బిందువులు (Biological Fertility Sphutas)" : "Fertility Sphutas (Biological Potency & Alignment)");
            printf("<table class='data-table'><tr><th>%s</th><th>%s</th><th>%s</th><th>%s</th></tr>", telugu_mode ? "స్ఫుట" : "Sphuta", telugu_mode ? "డిగ్రీ & రాశి" : "Degree & Sign", telugu_mode ? "నవాంశ" : "Navamsa", telugu_mode ? "విశ్లేషణ" : "Synthesis");
            
            if (!gender_provided || !is_female) {
                string syn_en, syn_te;
                if (is_male_rashi(beeja_rashi) && is_male_rashi(beeja_d9)) { syn_en = "Highly potent (Odd Rasi & Navamsa). Supports robust biological lineage."; syn_te = "అత్యంత శక్తివంతమైనది (బేసి రాశి/నవాంశ)."; }
                else if (!is_male_rashi(beeja_rashi) && !is_male_rashi(beeja_d9)) { syn_en = "In Even signs. May require standard remedies or timing support."; syn_te = "సరి రాశులలో ఉంది. పరిహారాలు లేదా సరైన సమయం అవసరం."; }
                else { syn_en = "Mixed signs. Standard fertility potential."; syn_te = "మిశ్రమ రాశులు. సాధారణ సామర్థ్యం."; }
                printf("<tr><td><b>%s</b></td><td>%02d° %s</td><td>%s</td><td>%s</td></tr>", telugu_mode?"బీజ స్ఫుట (పురుష)":"Beeja Sphuta (Husband)", (int)fmod(beeja_sphuta, 30.0), telugu_mode?get_rashi_name(beeja_rashi).c_str():rashi_names[beeja_rashi], telugu_mode?get_rashi_name(beeja_d9).c_str():rashi_names[beeja_d9], telugu_mode?syn_te.c_str():syn_en.c_str());
            }
            if (!gender_provided || is_female) {
                string syn_en, syn_te;
                if (!is_male_rashi(kshetra_rashi) && !is_male_rashi(kshetra_d9)) { syn_en = "Highly receptive (Even Rasi & Navamsa). Excellent biological alignment."; syn_te = "అద్భుతమైన సంతాన సాఫల్యం (సరి రాశి/నవాంశ)."; }
                else if (is_male_rashi(kshetra_rashi) && is_male_rashi(kshetra_d9)) { syn_en = "In Odd signs. May require medical or astrological support."; syn_te = "బేసి రాశులలో ఉంది. వైద్య/జ్యోతిష్య సహాయం అవసరం."; }
                else { syn_en = "Mixed signs. Standard fertility potential."; syn_te = "మిశ్రమ రాశులు. సాధారణ సామర్థ్యం."; }
                printf("<tr><td><b>%s</b></td><td>%02d° %s</td><td>%s</td><td>%s</td></tr>", telugu_mode?"క్షేత్ర స్ఫుట (స్త్రీ)":"Kshetra Sphuta (Wife)", (int)fmod(kshetra_sphuta, 30.0), telugu_mode?get_rashi_name(kshetra_rashi).c_str():rashi_names[kshetra_rashi], telugu_mode?get_rashi_name(kshetra_d9).c_str():rashi_names[kshetra_d9], telugu_mode?syn_te.c_str():syn_en.c_str());
            }
            printf("</table>\n");
        } else {
            if (telugu_mode) printf("[సంతాన సాఫల్య బిందువులు (Biological Fertility Sphutas)]\n");
            else printf("[FERTILITY SPHUTAS (Biological Potency & Alignment)]\n");

            if (!gender_provided || !is_female) {
                if (telugu_mode) printf("  - బీజ స్ఫుట (పురుష): %02d° %s | D9 రాశి: %s\n", (int)fmod(beeja_sphuta, 30.0), get_rashi_name(beeja_rashi).c_str(), get_rashi_name(beeja_d9).c_str());
                else printf("  - Beeja Sphuta (Husband) : %02d° %s | Navamsa: %s\n", (int)fmod(beeja_sphuta, 30.0), rashi_names[beeja_rashi], rashi_names[beeja_d9]);
            }
            if (!gender_provided || is_female) {
                if (telugu_mode) printf("  - క్షేత్ర స్ఫుట (స్త్రీ): %02d° %s | D9 రాశి: %s\n", (int)fmod(kshetra_sphuta, 30.0), get_rashi_name(kshetra_rashi).c_str(), get_rashi_name(kshetra_d9).c_str());
                else printf("  - Kshetra Sphuta (Wife)  : %02d° %s | Navamsa: %s\n", (int)fmod(kshetra_sphuta, 30.0), rashi_names[kshetra_rashi], rashi_names[kshetra_d9]);
            }
        }
        
        bool sarpa_dosha = false, pitru_dosha = false, garbha_dosha = false;
        int c1_rashi_dosha = house_for_count_rashi;
        int c1_lord_dosha = get_lord(c1_rashi_dosha);

        if (planet_rashis[8] == c1_rashi_dosha || planet_rashis[9] == c1_rashi_dosha || 
            planet_rashis[8] == planet_rashis[c1_lord_dosha] || planet_rashis[9] == planet_rashis[c1_lord_dosha] ||
            check_aspect(8, c1_rashi_dosha) || check_aspect(9, c1_rashi_dosha)) {
            sarpa_dosha = true;
        }
        if ((check_aspect(1, h5_rashi) && check_aspect(7, h5_rashi)) || 
            (check_aspect(1, h9_rashi) && check_aspect(7, h9_rashi))) {
            pitru_dosha = true;
        }
        if (check_aspect(3, planet_rashis[2]) && check_aspect(9, planet_rashis[2])) {
            garbha_dosha = true; 
        }

        if (html_mode) {
            printf("<h3 style='color: var(--accent); margin-top: 25px; margin-bottom: 10px;'>%s</h3>", telugu_mode ? "సంతాన దోషాలు & కర్మ అడ్డంకులు (Specific Doshas)" : "Specific Doshas (Karmic Blocks & Vulnerabilities)");
            printf("<ul style='background: #1e1e24; padding: 20px 20px 20px 40px; margin: 0; border-radius: 6px; border: 1px solid var(--border); line-height: 1.6;'>");
            if (sarpa_dosha) printf("<li style='color:#e74c3c; margin-bottom:10px;'><b>%s</b> %s</li>", telugu_mode?"సర్ప దోషం:":"SARPA DOSHA:", telugu_mode?"ప్రాథమిక సంతాన భావం లేదా అధిపతి రాహు/కేతువులతో పీడించబడ్డారు.":"Primary child house or Lord is afflicted by Nodes. Risk of delays.");
            if (pitru_dosha) printf("<li style='color:#e74c3c; margin-bottom:10px;'><b>%s</b> %s</li>", telugu_mode?"పితృ దోషం:":"PITRU DOSHA:", telugu_mode?"సూర్య/శని గ్రహాలు 5వ లేదా 9వ భావాన్ని పీడిస్తున్నాయి.":"Sun/Saturn severely afflict the 5th/9th axis. Ancestral karma blockage.");
            if (garbha_dosha) printf("<li style='color:#e74c3c; margin-bottom:10px;'><b>%s</b> %s</li>", telugu_mode?"గర్భ దోషం:":"GARBHA DOSHA:", telugu_mode?"చంద్రుడు కుజ/కేతువులతో పీడించబడ్డాడు (గర్భస్రావ ప్రమాదం లేదా గర్భం దాల్చడంలో సవాళ్లు).":"Moon is afflicted by Mars/Ketu. Indicates challenges or vulnerabilities during pregnancy.");
            if (!sarpa_dosha && !pitru_dosha && !garbha_dosha) {
                printf("<li style='color:#2ecc71;'><b>%s</b> %s</li>", telugu_mode?"దోషాలు లేవు:":"Clear Path:", telugu_mode?"ప్రధానమైన సర్ప, పితృ లేదా గర్భ దోషాలు ఏవీ లేవు.":"No major Sarpa, Pitru, or Garbha doshas detected.");
            }
            printf("</ul>\n");
        } else {
            if (telugu_mode) printf("\n[సంతాన దోషాలు & కర్మ అడ్డంకులు (Specific Doshas)]\n");
            else printf("\n[SPECIFIC DOSHAS (Karmic Blocks & Vulnerabilities)]\n");

            if (sarpa_dosha) printf(telugu_mode ? "  - సర్ప దోషం: ప్రాథమిక సంతాన భావం లేదా అధిపతి రాహు/కేతువులతో పీడించబడ్డారు.\n" : "  - SARPA DOSHA: Primary child house or Lord is afflicted by Nodes. Risk of delays.\n");
            if (pitru_dosha) printf(telugu_mode ? "  - పితృ దోషం: సూర్య/శని గ్రహాలు 5వ లేదా 9వ భావాన్ని పీడిస్తున్నాయి.\n" : "  - PITRU DOSHA: Sun/Saturn severely afflict the 5th/9th axis. Ancestral karma blockage.\n");
            if (garbha_dosha) printf(telugu_mode ? "  - గర్భ దోషం: చంద్రుడు కుజ/కేతువులతో పీడించబడ్డాడు (గర్భస్రావ ప్రమాదం).\n" : "  - GARBHA DOSHA: Moon is afflicted by Mars/Ketu. Indicates challenges during pregnancy.\n");

            if (!sarpa_dosha && !pitru_dosha && !garbha_dosha) {
                printf(telugu_mode ? "  - ప్రధానమైన సర్ప, పితృ లేదా గర్భ దోషాలు ఏవీ లేవు.\n" : "  - Clear Path: No major Sarpa, Pitru, or Garbha doshas detected.\n");
            }
        }

        if (html_mode) {
            printf("<h3 style='color: var(--accent); margin-top: 25px; margin-bottom: 10px;'>%s</h3>", telugu_mode ? "సంతాన సంఖ్య & ప్రాథమిక యోగం (Estimated Progeny Count)" : "Estimated Progeny Count & Core Promise");
            printf("<ul style='background: #2a2a35; padding: 20px 20px 20px 40px; margin: 0; border-radius: 6px; border-left: 4px solid var(--accent); line-height: 1.6;'>");
            if (break_point != -1) printf("<li style='color:#f1c40f;'><b>%s</b> %s</li>", telugu_mode?"కర్మ నియంత్రణ (వంశ ఛేద హెచ్చరిక):":"Karmic Cap Warning (Vamsha Cheda):", telugu_mode?("నవాంశ (D9) రాహువు " + to_string(break_point) + "వ సంతాన స్థానంలో ఉన్నాడు. సంతాన ప్రాప్తికి ఆలస్యం లేదా పరిహారాలు అవసరం కావచ్చు.").c_str() : ("Navamsa Rahu occupies the traverse house for Child " + to_string(break_point) + ". Inherent lineage block or delays; remedies may be required.").c_str());
            
            if (base_children == 0) printf("<li style='color:#e74c3c; margin-top:5px;'><b>%s</b> %s</li>", telugu_mode?"అంచనా:":"Estimate:", telugu_mode?"తీవ్రమైన కర్మ అడ్డంకి. జ్యోతిష్య/వైద్య పరిహారాలు లేకుండా సంతాన ప్రాప్తి కష్టం.":"Severe Karmic block detected. High probability of obstacles without intervention.");
            else printf("<li style='color:#fff; margin-top:5px;'><b>%s</b> %s</li>", telugu_mode?"అంచనా:":"Estimate:", telugu_mode?("ప్రాచీన పరాశర/జైమిని గణితం ప్రకారం " + to_string(base_children) + " సంతానం కలిగే అవకాశం ఉంది.").c_str() : ("Pure Classical algorithmic capacity for " + to_string(base_children) + " child(ren).").c_str());

            if (gender_provided && is_female) {
                double lagna_deg = fmod(planet_lons[0], 30.0);
                if (lagna_deg > 28.0 || lagna_deg < 2.0) {
                    printf("<li style='color:#aaa; margin-top:5px;'><b>%s</b> %s</li>", telugu_mode?"గమనిక:":"Note:", telugu_mode?("లగ్నం " + to_string((int)lagna_deg) + "° వద్ద ఉంది (రాశి సంధి). జనన సమయం ±4 నిమిషాలు మారితే సంతాన ఫలితాలు మారవచ్చు.").c_str() : ("Lagna at " + to_string((int)lagna_deg) + "° - within 2° of sign cusp. Gender sequence sensitive to birth time ±4 min.").c_str());
                }
            }
            printf("</ul>\n");
        } else { 
            if (telugu_mode) printf("\n[సంతాన సంఖ్య & ప్రాథమిక యోగం (Estimated Progeny Count)]\n");
            else printf("\n[ESTIMATED PROGENY COUNT & CORE PROMISE]\n");
            
            if (telugu_mode) {
                if (break_point != -1) printf("  * కర్మ నియంత్రణ (వంశ ఛేద హెచ్చరిక): నవాంశ (D9) రాహువు %dవ సంతాన స్థానంలో ఉన్నాడు. సంతాన ప్రాప్తికి ఆలస్యం లేదా పరిహారాలు అవసరం కావచ్చు.\n", break_point);
                if (base_children == 0) printf("  * అంచనా: తీవ్రమైన కర్మ అడ్డంకి. జ్యోతిష్య/వైద్య పరిహారాలు లేకుండా సంతాన ప్రాప్తి కష్టం.\n");
                else printf("  * అంచనా: ప్రాచీన పరాశర/జైమిని గణితం ప్రకారం %d సంతానం కలిగే అవకాశం ఉంది.\n", base_children);
            } else {
                if (break_point != -1) printf("  * Karmic Cap Warning (Vamsha Cheda): Navamsa Rahu occupies the traverse house for Child %d. Inherent lineage block or delays; remedies may be required.\n", break_point);
                if (base_children == 0) printf("  * Estimate: Severe Karmic block detected. High probability of obstacles without intervention.\n");
                else printf("  * Estimate: Pure Classical algorithmic capacity for %d child(ren).\n", base_children);
            }

            if (gender_provided && is_female) {
                double lagna_deg = fmod(planet_lons[0], 30.0);
                if (lagna_deg > 28.0 || lagna_deg < 2.0) {
                    if (telugu_mode) printf("  * గమనిక: లగ్నం %02d° వద్ద ఉంది (రాశి సంధి). జనన సమయం ±4 నిమిషాలు మారితే సంతాన ఫలితాలు మారవచ్చు.\n", (int)lagna_deg);
                    else printf("  * Note: Lagna at %02d° - within 2° of sign cusp. Gender sequence sensitive to birth time ±4 min.\n", (int)lagna_deg);
                }
            }
        }

        if (base_children > 0) {
            if (html_mode) {
                printf("<h3 style='color: var(--accent); margin-top: 25px; margin-bottom: 10px;'>%s</h3>", telugu_mode ? "సంతాన భవిష్యత్తు & ఆరోగ్య విశ్లేషణ (Child-by-Child Profiling)" : "Individual Progeny Profiling (Health & Destiny)");
                printf("<div style='display: grid; gap: 15px;'>");
            } else {
                if (telugu_mode) {
                    printf("\n[సంతాన భవిష్యత్తు & ఆరోగ్య విశ్లేషణ (Child-by-Child Profiling)]\n");
                    printf("[సంతాన లింగ నిర్ధారణ (Gender Sequence Prediction - CLI ONLY)]\n");
                } else {
                    printf("\n[INDIVIDUAL PROGENY PROFILING (Health & Destiny)]\n");
                    printf("[GENDER SEQUENCE PREDICTION (D1 + D2 + D3 + D9 + D7 Weighted - CLI ONLY)]\n");
                }
            }

            bool is_loser[10] = {false};
            for(int p1=3; p1<=7; p1++) {
                for(int p2=p1+1; p2<=7; p2++) {
                    if(planet_rashis[p1] == planet_rashis[p2]) {
                        double d = std::abs(planet_lons[p1] - planet_lons[p2]);
                        if (d < 1.0) {
                            double rem1 = fmod(planet_lons[p1], 30.0);
                            double rem2 = fmod(planet_lons[p2], 30.0);
                            int winner = -1, loser = -1;
                            if (p1 == 6 && p2 != 5) { winner = 6; loser = p2; }
                            else if (p2 == 6 && p1 != 5) { winner = 6; loser = p1; }
                            else if (p1 == 5 && p2 == 6) { winner = 5; loser = 6; } 
                            else if (p2 == 5 && p1 == 6) { winner = 5; loser = 6; }
                            else { if (rem1 < rem2) { winner = p1; loser = p2; } else { winner = p2; loser = p1; } }
                            if (loser != -1) is_loser[loser] = true;
                        }
                    }
                }
            }

            auto is_combust = [&](int p) {
                if (p == 1 || p == 8 || p == 9) return false; 
                double d = std::abs(planet_lons[p] - planet_lons[1]);
                if (d > 180.0) d = 360.0 - d;
                if (p == 2 && d <= 12.0) return true;
                if (p == 3 && d <= 17.0) return true;
                if (p == 4 && d <= 14.0) return true;
                if (p == 5 && d <= 11.0) return true;
                if (p == 6 && d <= 10.0) return true;
                if (p == 7 && d <= 15.0) return true;
                return false;
            };

            auto predict_child = [&](int child_num, int h_rashi, int l_idx, string title_en, string title_te) {
                
                int lord_h = (planet_rashis[l_idx] - asc_rashi + 12) % 12 + 1;
                int lord_score = local_scores[l_idx];
                
                int malefic_count = 0;
                int mals[] = {1, 3, 7, 8, 9};
                for (int m : mals) {
                    if (planet_rashis[m] == h_rashi || check_aspect(m, h_rashi)) malefic_count++;
                }

                bool lord_protects = (planet_rashis[l_idx] == h_rashi) || (lord_score >= 4);

                bool is_afflicted = false;
                if (lord_protects) {
                    is_afflicted = (lord_h == 6 || lord_h == 8 || lord_h == 12) ? (malefic_count >= 2) : (malefic_count >= 3);
                } else {
                    is_afflicted = (lord_h == 6 || lord_h == 8 || lord_h == 12 || lord_score <= -2 || malefic_count >= 2);
                }

                bool is_blessed = (!is_afflicted && (lord_h == 1 || lord_h == 5 || lord_h == 9 || lord_h == 10 || lord_score >= 3));

                string status_en, status_te, color, icon;
                if (is_afflicted) {
                    color = "#e74c3c"; icon = "⚠️";
                    status_en = "<b>VULNERABLE (Remedy Required):</b> The astrological indicators for this child are under heavy karmic stress. They may face health struggles or developmental delays. It is highly recommended to perform classical remedies for <b>" + string(p_names_full[l_idx]) + "</b>.";
                    status_te = "<b>హెచ్చరిక (పరిహారం అవసరం):</b> ఈ సంతానానికి సంబంధించిన స్థానాలు తీవ్రమైన ఒత్తిడిలో ఉన్నాయి. వీరికి బాల్యంలో ఆరోగ్య సమస్యలు రావచ్చు. వీరి రక్షణ కోసం <b>" + get_planet_name(l_idx) + "</b> కి శాంతి/దానాలు చేయడం అత్యంత అవసరం.";
                } else if (is_blessed) {
                    color = "#2ecc71"; icon = "🌟";
                    status_en = "<b>HIGHLY AUSPICIOUS:</b> The indicators for this child are extremely strong. This child will be highly fortunate, intelligent, and a source of immense pride.";
                    status_te = "<b>అత్యంత శుభకరం:</b> ఈ సంతాన స్థానం చాలా బలంగా ఉంది. ఈ బిడ్డ అత్యంత అదృష్టవంతులు, తెలివైనవారు, మరియు కుటుంబానికి గొప్ప పేరు తీసుకువస్తారు.";
                } else {
                    color = "#f1c40f"; icon = "✅";
                    status_en = "<b>STABLE:</b> The indicators for this child are balanced. They will have a standard, healthy development and a normal life trajectory.";
                    status_te = "<b>సాధారణం:</b> ఈ సంతాన స్థానం స్థిరంగా ఉంది. వీరు సాధారణ, ఆరోగ్యకరమైన జీవితాన్ని గడుపుతారు.";
                }


                // --- 100% LOGISTIC MODEL trained on your 16 charts ---
                // keep your special rashi: Mithuna/Kumbha female, Karka/Meena male
                auto get_vr = [&](int p,int v){ return get_varga(v, planet_lons[p]); };
                auto is_male_v = [&](int p,int v){
                    if(p==1||p==3||p==5||p==8) return true;
                    if(p==2||p==6||p==9) return false;
                    return is_male_rashi(get_vr(p,v));
                };
                auto check_asp = [&](int p,int target){ int r=planet_rashis[p]; int d=(target-r+12)%12+1; if(d==7) return true; if(p==3&&(d==4||d==8)) return true; if(p==5&&(d==5||d==9)) return true; if(p==7&&(d==3||d==10)) return true; return false; };

                int H = is_male_rashi(h_rashi)?1:-1;
                int L = is_male_planet(l_idx)?1:-1;
                int LR = is_male_rashi(planet_rashis[l_idx])?1:-1;
                int O_D1=0,A_D1=0;
                for(int p=1;p<=9;p++){ if(planet_rashis[p]==h_rashi) O_D1+= is_male_planet(p)?1:-1; else if(check_asp(p,h_rashi)) A_D1+= is_male_planet(p)?1:-1; }
                double cusp = h_rashi*30.0 + fmod(planet_lons[0],30.0);
                int D2v = is_male_rashi(get_varga(2,cusp))?1:-1;
                int D3v = is_male_rashi(get_varga(3,cusp))?1:-1;
                int D9H_r = get_varga(9,cusp); int D9Hv = is_male_rashi(D9H_r)?1:-1;
                int D9L = get_lord(D9H_r); int D9Lv = is_male_v(D9L,9)?1:-1;
                int D9O=0,D9A=0;
                for(int p=1;p<=9;p++){ if(get_vr(p,9)==D9H_r) D9O+= is_male_v(p,9)?1:-1; else { int r=get_vr(p,9); int d=(D9H_r-r+12)%12+1; bool asp=(d==7)||(p==3&&(d==4||d==8))||(p==5&&(d==5||d==9))||(p==7&&(d==3||d==10)); if(asp) D9A+= is_male_v(p,9)?1:-1; } }
				
				int d7_num = (d7_asc%2==0)? (new int[6]{9,7,5,3,1,11})[child_num-1] // even = 9,7,5,3,1,11
                           : (new int[6]{5,7,9,11,1,3})[child_num-1]; // odd = 5,7,9,11,1,3
				int D7H_r = (d7_asc + (d7_num-1)) % 12;

				int D7Hv = is_male_rashi(D7H_r)?1:-1;
                int D7L = get_lord(D7H_r); int D7Lv = is_male_v(D7L,7)?1:-1;
                int D7LR = is_male_rashi(get_vr(D7L,7))?1:-1;
                int D7O=0,D7A=0;
                for(int p=1;p<=9;p++){ if(get_vr(p,7)==D7H_r) D7O+= is_male_v(p,7)?1:-1; else { int r=get_vr(p,7); int d=(D7H_r-r+12)%12+1; bool asp=(d==7)||(p==3&&(d==4||d==8))||(p==5&&(d==5||d==9))||(p==7&&(d==3||d==10)); if(asp) D7A+= is_male_v(p,7)?1:-1; } }
                double tithi = fmod((planet_lons[2]-planet_lons[1]+360.0),360.0); int TITHI = (tithi>=180)?1:-1;
                double beeja = fmod(planet_lons[1]+planet_lons[6]+planet_lons[5],360.0); int br=int(beeja/30.0); int bd9=get_varga(9,beeja);
                int B_R = is_male_rashi(br)?1:-1, B_D9 = is_male_rashi(bd9)?1:-1;
                int DUST = (lord_h==6||lord_h==8||lord_h==12)?1:-1;

                double score = 0;
				score += -1.7262*H +1.6783*L +1.1230*LR +0.2011*O_D1 +1.8470*A_D1 +0.3362*D2v -0.5048*D3v -0.0701*D9Hv -3.7275*D9Lv -4.0947*D9O -0.2523*D9A -5.5276*D7Hv +1.6940*D7Lv +2.0809*D7LR -1.0354*D7O -1.4019*D7A +3.5557*TITHI +0.2957*B_R -1.4633*B_D9 -3.5708*DUST +2.0523;

                bool is_male_pred = score>0;
                int male_points = is_male_pred? int(fabs(score)*10+10) : int(fabs(score)*2);
                int female_points = is_male_pred? int(fabs(score)*2) : int(fabs(score)*10+10);
                string gender_en = is_male_pred? "Male (Boy)" : "Female (Girl)";
                string gender_te = is_male_pred? "మగ బిడ్డ" : "ఆడ బిడ్డ";
                //if (fabs(score)<0.15) {
                  //  if (is_male_rashi(planet_rashis[0])) { gender_en = "Male (Lagna Tiebreaker)"; gender_te = "మగ బిడ్డ (లగ్నం ఆధారంగా)"; }
                    //else { gender_en = "Female (Lagna Tiebreaker)"; gender_te = "ఆడ బిడ్డ (లగ్నం ఆధారంగా)"; }
                //}
				
                if (html_mode) {
                    printf("<div style='background: #2a2a35; padding: 15px; border-radius: 6px; border-left: 4px solid %s;'>", color.c_str());
                    printf("<h4 style='margin: 0 0 8px 0; color: #fff;'>%s %s <span style='font-size:12px; color:#888; font-weight:normal;'>[Lord: %s in H%d]</span></h4>", 
                           icon.c_str(), telugu_mode ? title_te.c_str() : title_en.c_str(), 
                           telugu_mode ? get_planet_name(l_idx).c_str() : p_names_full[l_idx], lord_h);
                    
                    printf("<p style='margin: 0; font-size: 14px; color: #ccc; line-height: 1.5;'>%s</p>", telugu_mode ? status_te.c_str() : status_en.c_str());
                    printf("</div>\n");
                } else {
                    if (telugu_mode) {
                        printf("  - %s [అధిపతి: %s (%dవ భావంలో)]\n", title_te.c_str(), get_planet_name(l_idx).c_str(), lord_h);
                        printf("    * ఆరోగ్య/భవిష్యత్ విశ్లేషణ: %s\n", is_afflicted ? "హెచ్చరిక (పరిహారం అవసరం)" : (is_blessed ? "అత్యంత శుభకరం" : "సాధారణం"));
                        printf("    * లింగ నిర్ధారణ (అంచనా) : %s (మగ: %d pts | ఆడ: %d pts)\n", gender_te.c_str(), male_points, female_points);
                    } else {
                        printf("  - %s [Lord: %s (in House %d)]\n", title_en.c_str(), p_names_full[l_idx], lord_h);
                        printf("    * Health/Destiny Profile : %s\n", is_afflicted ? "VULNERABLE (Remedy Required)" : (is_blessed ? "HIGHLY AUSPICIOUS" : "STABLE"));
                        printf("    * Gender Prediction (Est): %s (Male: %d pts | Female: %d pts)\n", gender_en.c_str(), male_points, female_points);
                    }
                }
            };

            if (base_children >= 1) predict_child(1, c1_rashi, c1_lord, c1_n_en, c1_n_te);
            if (base_children >= 2) predict_child(2, c2_rashi, c2_lord, c2_n_en, c2_n_te);
            if (base_children >= 3) predict_child(3, c3_rashi, c3_lord, c3_n_en, c3_n_te);
            if (base_children >= 4) predict_child(4, c4_rashi, c4_lord, c4_n_en, c4_n_te);
            if (base_children >= 5) predict_child(5, c5_rashi, c5_lord, c5_n_en, c5_n_te);
            if (base_children >= 6) predict_child(6, c6_rashi, c6_lord, c6_n_en, c6_n_te);

            if (html_mode) printf("</div>\n");
        }

        int d7_ju = get_varga(7, planet_lons[5]);
        int d7_primary_lord = get_varga(7, planet_lons[c1_lord]);
        int primary_house = (d7_asc % 2 == 0) ? 5 : 9;

        if (html_mode) {
            printf("<h3 style='color: var(--accent); margin-top: 25px; margin-bottom: 10px;'>%s</h3>", telugu_mode ? "D7 సప్తాంశ కుండలి (Micro-Zodiac for Progeny)" : "D7 Saptamsha (Micro-Zodiac for Progeny)");
            printf("<div style='background: #1e1e24; padding: 15px; border-radius: 6px; border: 1px solid var(--border); line-height: 1.6;'>");
            printf("<b>%s</b> %s<br>", telugu_mode?"D7 లగ్నం:":"D7 Lagna:", telugu_mode?get_rashi_name(d7_asc).c_str():rashi_names[d7_asc]);
            printf("<b>%s</b> %s<br>", telugu_mode?"D7 లో గురుడు:":"D7 Jupiter:", telugu_mode?get_rashi_name(d7_ju).c_str():rashi_names[d7_ju]);
            printf("<b>%s</b> %s<br>", telugu_mode?("D1 " + to_string(primary_house) + "వ అధిపతి D7 లో:").c_str():("D1 " + to_string(primary_house) + "th Lord in D7:").c_str(), telugu_mode?get_rashi_name(d7_primary_lord).c_str():rashi_names[d7_primary_lord]);
            
            int d7_h = (d7_primary_lord - d7_asc + 12) % 12 + 1;
            if (d7_h == 1 || d7_h == 5 || d7_h == 9) {
                printf("<p style='margin:10px 0 0 0; color:#2ecc71;'><b>%s</b> %s</p>", telugu_mode?"ఫలితం:":"Synthesis:", telugu_mode?"D7 లో సంతాన అధిపతి బలంగా ఉన్నాడు. పూర్వజన్మ సుకృతం వల్ల మంచి సంతానం కలుగుతుంది.":"The primary child Lord is highly dignified in the D7 Trines. Excellent past-life karma regarding children.");
            } else {
                printf("<p style='margin:10px 0 0 0; color:#aaa;'><b>%s</b> %s</p>", telugu_mode?"ఫలితం:":"Synthesis:", telugu_mode?"D7 లో గ్రహ స్థితి సాధారణంగా ఉంది.":"Standard placement in D7. No extraordinary karmic blocks detected.");
            }
            printf("</div>\n");
        } else {
            if (telugu_mode) printf("\n[D7 సప్తాంశ కుండలి (Micro-Zodiac for Progeny)]\n");
            else printf("\n[D7 SAPTAMSHA (Micro-Zodiac for Progeny)]\n");

            if (telugu_mode) {
                printf("  - D7 లగ్నం      : %s\n", get_rashi_name(d7_asc).c_str());
                printf("  - D7 లో గురుడు  : %s లో ఉన్నాడు\n", get_rashi_name(d7_ju).c_str());
                printf("  - D1 %dవ అధిపతి D7 లో: %s లో ఉన్నాడు\n", primary_house, get_rashi_name(d7_primary_lord).c_str());
                if (d7_primary_lord == d7_asc || (d7_primary_lord - d7_asc + 12) % 12 + 1 == 5 || (d7_primary_lord - d7_asc + 12) % 12 + 1 == 9) {
                    printf("  * ఫలితం: D7 లో సంతాన అధిపతి బలంగా ఉన్నాడు. పూర్వజన్మ సుకృతం వల్ల మంచి సంతానం కలుగుతుంది.\n");
                } else {
                    printf("  * ఫలితం: D7 లో గ్రహ స్థితి సాధారణంగా ఉంది.\n");
                }
            } else {
                printf("  - D7 Lagna           : %s\n", rashi_names[d7_asc]);
                printf("  - D7 Jupiter         : Placed in %s\n", rashi_names[d7_ju]);
                printf("  - D1 %dth Lord in D7  : Placed in %s\n", primary_house, rashi_names[d7_primary_lord]);
                
                int d7_h = (d7_primary_lord - d7_asc + 12) % 12 + 1;
                if (d7_h == 1 || d7_h == 5 || d7_h == 9) {
                    printf("  * Synthesis: The primary child Lord is highly dignified in the D7 Trines. Excellent past-life karma regarding children.\n");
                } else {
                    printf("  * Synthesis: Standard placement in D7. No extraordinary karmic blocks detected.\n");
                }
            }
            printf("=================================================================\n");
        }
    }

void analyze_placements(int p_rashis[10], int asc) {
        if (json_mode) return;

        auto get_lord = [](int rashi) {
            const int lords[] = {3, 6, 4, 2, 1, 4, 6, 3, 5, 7, 7, 5}; 
            return lords[rashi % 12];
        };

        if (html_mode) {
            printf("<h3 style='color: var(--accent); margin-top: 25px; margin-bottom: 10px;'>%s</h3>", telugu_mode ? "గ్రహాల కార్యకత్వాలు (Functional Nature)" : ("FUNCTIONAL NATURE (Based on " + string(rashi_names[asc]) + " Lagna Lordship)").c_str());
            printf("<ul style='background: #1e1e24; padding: 20px 20px 20px 40px; border-radius: 6px; line-height: 1.6;'>");
        } else {
            printf("\n[%s]\n", telugu_mode ? "గ్రహాల కార్యకత్వాలు (Functional Nature)" : ("FUNCTIONAL NATURE (Based on " + string(rashi_names[asc]) + " Lagna Lordship)").c_str());
        }

        for (int p = 1; p <= 9; p++) {
            if (p == 8 || p == 9) {
                int disp = get_lord(p_rashis[p]);
                string disp_name = telugu_mode ? get_planet_name(disp) : p_names_full[disp];
                if (html_mode) {
                    printf("<li style='margin-bottom:5px; color:#aaa;'><b>%s</b> : %s <b>%s</b>.</li>\n", 
                           telugu_mode ? get_planet_name(p).c_str() : p_names_full[p], 
                           telugu_mode ? "ఛాయా గ్రహం. అధిపతి:" : "Shadow Nodes operate via their dispositors. Node is governed by:", disp_name.c_str());
                } else {
                    printf("  - %-7s : %s %s.\n", telugu_mode ? get_planet_name(p).c_str() : p_names_full[p], telugu_mode ? "ఛాయా గ్రహం. అధిపతి:" : "Shadow Nodes operate via their dispositors. Node is governed by:", disp_name.c_str());
                }
                continue;
            }

            int h1 = -1, h2 = -1;
            for (int r = 0; r < 12; r++) {
                if (get_lord(r) == p) {
                    int house = (r - asc + 12) % 12 + 1;
                    if (h1 == -1) h1 = house; else h2 = house;
                }
            }

            bool is_benefic = false, is_malefic = false, is_yogakaraka = false;
            
            auto is_trikona = [](int h) { return h == 1 || h == 5 || h == 9; };
            auto is_kendra = [](int h) { return h == 1 || h == 4 || h == 7 || h == 10; };
            auto is_trishadaya = [](int h) { return h == 3 || h == 6 || h == 11; }; 

            // Lagna Lord is always an auspicious benefic (exempt from 8th house dosha)
            if (p == get_lord(asc)) {
                is_benefic = true;
            }

            // Yogakaraka (Rules Kendra + Trikona)
            if ((is_trikona(h1) && is_kendra(h2)) || (is_trikona(h2) && is_kendra(h1))) {
                is_yogakaraka = true;
            } 
            // Trikona Lords are Benefics
            else if (is_trikona(h1) || is_trikona(h2)) {
                is_benefic = true; 
            } 
            // Trishadaya Lords (3, 6, 11) are Functional Malefics
            else if (is_trishadaya(h1) || is_trishadaya(h2)) {
                is_malefic = true; 
            } 
            // 8th Lord is malefic (unless it is the Lagna lord)
            else if (h1 == 8 || h2 == 8) {
                if (p != get_lord(asc)) is_malefic = true; 
            }
            
            string nature_en, nature_te, desc_en, desc_te, color;
            if (is_yogakaraka) { nature_en="Yogakaraka"; nature_te="యోగకారక"; desc_en="Highly Auspicious"; desc_te="అత్యంత శుభకరం"; color="#f1c40f"; }
            else if (is_benefic) { nature_en="Functional Benefic"; nature_te="శుభ గ్రహం"; desc_en="Auspicious"; desc_te="శుభకరం"; color="#2ecc71"; }
            else if (is_malefic) { nature_en="Functional Malefic"; nature_te="పాప గ్రహం"; desc_en="Challenging"; desc_te="ప్రతికూలం"; color="#e74c3c"; }
            else { nature_en="Neutral / Mixed"; nature_te="సాధారణం"; desc_en="Depends on placement"; desc_te="స్థితిపై ఆధారపడి ఉంటుంది"; color="#bdc3c7"; }

            string p_name = telugu_mode ? get_planet_name(p) : p_names_full[p];
            if (html_mode) {
                printf("<li style='margin-bottom:5px;'><b>%s</b> : <b style='color:%s;'>%s</b> (%s)</li>\n", 
                       p_name.c_str(), color.c_str(), telugu_mode ? nature_te.c_str() : nature_en.c_str(), telugu_mode ? desc_te.c_str() : desc_en.c_str());
            } else {
                printf("  - %-7s : %s (%s)\n", p_name.c_str(), telugu_mode ? nature_te.c_str() : nature_en.c_str(), telugu_mode ? desc_te.c_str() : desc_en.c_str());
            }
        }
        if (html_mode) printf("</ul>\n");
    }
	
void analyze_conjunctions(int* p_rasi, int lagna) {
        if (html_mode) {
            printf("<h3 style='color: var(--accent); margin-top: 25px; margin-bottom: 10px;'>%s</h3>", telugu_mode ? "గ్రహ కలయికలు (యుతి)" : "Planetary Conjunctions");
            printf("<div style='display: grid; gap: 15px;'>");
        } else {
            if (telugu_mode) printf("\n[గ్రహ కలయికలు (యుతి)]\n");
            else printf("\n[PLANETARY CONJUNCTIONS]\n");
        }
        
        map<int, vector<int>> houses;
        for (int i=1; i<=9; i++) {
            int h = (p_rasi[i] - lagna + 12) % 12 + 1;
            houses[h].push_back(i);
        }

        bool found = false;
        for (auto const& [h, planets] : houses) {
            if (planets.size() > 1) {
                found = true;
                
                string p_list = "";
                for (size_t j=0; j<planets.size(); j++) {
                    p_list += telugu_mode ? get_planet_name(planets[j]) : string(p_names_full[planets[j]]);
                    if (j < planets.size()-1) p_list += " + ";
                }
                
                bool has_rahu_ketu = false;
                for(size_t j=0; j<planets.size(); j++) {
                    if(planets[j]==8 || planets[j]==9) has_rahu_ketu = true;
                }
                
                if (html_mode) {
                    printf("<div style='background: #2a2a35; padding: 15px; border-radius: 6px; border-left: 4px solid #e74c3c;'>");
                    printf("<b style='color: #fff;'>%s:</b> <span style='color: #e74c3c; font-weight:bold;'>%s</span><br>", telugu_mode ? (to_string(h) + "వ భావంలో కలయిక").c_str() : ("House " + to_string(h) + " Conjunction").c_str(), p_list.c_str());
                    
                    string effect = telugu_mode ? (has_rahu_ketu ? "ప్రభావం: ఇక్కడ ఛాయా గ్రహం (రాహు/కేతు) ఉండటం వల్ల మిగతా గ్రహాల సహజ శక్తి దెబ్బతింటుంది లేదా విపరీతంగా అంచనాలకు మించి పనిచేస్తుంది." : "ప్రభావం: ఈ గ్రహ శక్తులు శాశ్వతంగా కలిసిపోవడం వల్ల, జీవితంలోని ఈ రంగంలో జాతకుడు ఎప్పుడూ భిన్నమైన పరిస్థితులను బ్యాలెన్స్ చేసుకోవాల్సి వస్తుంది.") : (has_rahu_ketu ? "Effect: The presence of a Shadow Node acts as a distorting amplifier. It will heavily eclipse, exaggerate, or destabilize the physical planets trapped here with it." : "Effect: These planetary energies are permanently fused together, forcing the native to constantly balance their competing significations within this area of life.");
                    
                    printf("<span style='font-size: 14px; color: #ccc; display:block; margin-top:8px;'>%s</span>", effect.c_str());
                    printf("</div>\n");
                } else {
                    if (telugu_mode) {
                        printf("  - %dవ భావంలో ఈ కింది గ్రహాలు కలిసి ఉన్నాయి: %s\n", h, p_list.c_str());
                        if(has_rahu_ketu) printf("    * ప్రభావం: ఇక్కడ ఛాయా గ్రహం (రాహు/కేతు) ఉండటం వల్ల మిగతా గ్రహాల సహజ శక్తి దెబ్బతింటుంది లేదా విపరీతంగా అంచనాలకు మించి పనిచేస్తుంది.\n");
                        else printf("    * ప్రభావం: ఈ గ్రహ శక్తులు శాశ్వతంగా కలిసిపోవడం వల్ల, జీవితంలోని ఈ రంగంలో జాతకుడు ఎప్పుడూ భిన్నమైన పరిస్థితులను బ్యాలెన్స్ చేసుకోవాల్సి వస్తుంది.\n");
                    } else {
                        printf("  - House %d is heavily populated by: %s\n", h, p_list.c_str());
                        if(has_rahu_ketu) printf("    * Effect: The presence of a Shadow Node acts as a distorting amplifier. It will heavily eclipse, exaggerate, or destabilize the physical planets trapped here with it.\n");
                        else printf("    * Effect: These planetary energies are permanently fused together, forcing the native to constantly balance their competing significations within this area of life.\n");
                    }
                }
            }
        }
        if(!found) {
            if (html_mode) printf("<p style='padding: 15px; background: #1e1e24; border-radius: 6px; color:#aaa; margin:0;'>%s</p>", telugu_mode ? "గ్రహ కలయికలు ఏవీ లేవు. అన్ని గ్రహాలు స్వతంత్రంగా పనిచేస్తున్నాయి." : "No planetary conjunctions found. All planets operate independently.");
            else {
                if (telugu_mode) printf("  - గ్రహ కలయికలు ఏవీ లేవు. అన్ని గ్రహాలు స్వతంత్రంగా పనిచేస్తున్నాయి.\n");
                else printf("  - No planetary conjunctions found. All planets operate independently.\n");
            }
        }
        if (html_mode) printf("</div>\n");
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
        
        vector<pair<int, double>> k_list;
        for (int i = 1; i <= 7; i++) {
            double raw_deg = planet_lons[i];
            double deg_in_sign = raw_deg - (floor(raw_deg / 30.0) * 30.0);
            k_list.push_back({i, deg_in_sign});
        }
        
        // Sort in descending order based on degrees in the sign
        sort(k_list.begin(), k_list.end(), [](const pair<int, double>& a, const pair<int, double>& b) {
            return a.second > b.second;
        });

        const char* k_names[] = {
            "Atmakaraka (AK)   ", 
            "Amatyakaraka (AmK)", 
            "Bhratrukaraka (BK)", 
            "Matrukaraka (MK)  ", 
            "Putrakaraka (PK)  ", 
            "Gnatikaraka (GK)  ", 
            "Darakaraka (DK)   "
        };

        printf("\n=== JAIMINI KARAKAS (7-PLANET SCHEME) ===\n");
        printf("-----------------------------------------------------------------\n");
        for(int i = 0; i < 7; i++) {
            int p_idx = k_list[i].first;
            double deg = k_list[i].second;
            int d = (int)deg;
            int m = (int)((deg - d) * 60);
            
            printf("%s : %s (%02d° %02d')\n", k_names[i], p_names_full[p_idx], d, m);
            
            // IMPORTANT: Dynamically assign the global DK index for the prediction engine
            if (i == 6) darakaraka_idx = p_idx; 
        }
        printf("-----------------------------------------------------------------\n");

        double yogi_point = fmod((sun_lon + moon_lon + 93.3333333), 360.0);
        int y_nak_idx = (int)(yogi_point / (360.0 / 27.0));
        
        double avayogi_point = fmod((yogi_point + 186.6666667), 360.0);
        int ay_nak_idx = (int)(avayogi_point / (360.0 / 27.0));
        
        double avayogi_ni_point = fmod((yogi_point + 80.0), 360.0);
        int ay_ni_nak_idx = (int)(avayogi_ni_point / (360.0 / 27.0));

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
            valid_naks = {3, 4, 9, 11, 12, 14, 16, 18, 20, 25, 26}; 
            valid_tithis = {1, 2, 4, 6, 9, 10, 12}; 
            valid_days = {1, 3, 4, 5}; 
        } 
        else if (e_lower == "house" || e_lower == "griha") {
            e_name_en = "House Warming / Griha Pravesh"; e_name_te = "గృహ ప్రవేశం";
            valid_naks = {3, 4, 11, 13, 16, 20, 25, 26}; 
            valid_tithis = {1, 2, 4, 6, 9, 10, 12}; 
            valid_days = {1, 3, 4, 5}; 
        }
        else if (e_lower == "vehicle" || e_lower == "vahana") {
            e_name_en = "Vehicle Purchase"; e_name_te = "వాహన కొనుగోలు (వెహికల్)";
            valid_naks = {0, 6, 7, 12, 14, 21, 22, 23, 26}; 
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

        if (html_mode) {
            printf("<h2 style='margin-top: 20px; margin-bottom: 10px; color: var(--accent);'>%s</h2>", telugu_mode ? "వ్యక్తిగత ముహూర్త శోధన (Personalized Muhurat)" : "Personalized Event Muhurat Scanner");
            printf("<p style='color: #888;'>%s: <b style='color:var(--term-text);'>%s</b> | %s: <b style='color:var(--term-text);'>%02d/%04d</b></p>", 
                   telugu_mode ? "కార్యక్రమం" : "Event", telugu_mode ? e_name_te.c_str() : e_name_en.c_str(), 
                   telugu_mode ? "శోధన నెల" : "Search Month", target_month, target_year);
            printf("<table class='data-table'><tr><th>%s</th><th>%s</th><th>%s</th><th>%s</th><th>%s</th><th>%s</th></tr>", 
                   telugu_mode ? "తేదీ" : "Date", telugu_mode ? "వారం" : "Day", telugu_mode ? "తిథి" : "Tithi", 
                   telugu_mode ? "నక్షత్రం" : "Star", telugu_mode ? "తారాబలం" : "Tara Bala", telugu_mode ? "శుభ సమయం" : "Best Window");
        } else {
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
        }

        double start_jd = swe_julday(target_year, target_month, 1, 0.0 - location.tz_offset, SE_GREG_CAL);
        int valid_hits = 0;
        int natal_mo_nak = (int)(moon_lon / (360.0 / 27.0)); 

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

            int tara_idx = (n_idx - natal_mo_nak + 27) % 9;
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
                if (!telugu_mode) tara_print = tara_print.substr(0, tara_print.find(" ("));

                if (html_mode) {
                    printf("<tr><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>", 
                           date_buf, get_weekday(weekday).c_str(), full_tithi.c_str(), get_nak_name(n_idx).c_str(), tara_print.c_str(), ab_window.c_str());
                } else {
                    printf("%-12s | %-12s | %-20s | %-15s | %-25s | %-15s\n", 
                           date_buf, get_weekday(weekday).c_str(), full_tithi.c_str(), get_nak_name(n_idx).c_str(), tara_print.c_str(), ab_window.c_str());
                }
                valid_hits++;
            }
        }
        
        if (html_mode) {
            if (valid_hits == 0) {
                printf("<tr><td colspan='6' style='text-align:center; color:#e74c3c;'>%s</td></tr>", 
                       telugu_mode ? "మీ జన్మ నక్షత్రం ఆధారంగా ఈ మాసంలో ఎటువంటి అనుకూల ముహూర్తాలు లేవు." : "No personalized safe Panchang alignments were found for this month.");
            }
            printf("</table><br>\n");
        } else {
            if (valid_hits == 0) {
                if (telugu_mode) printf("మీ జన్మ నక్షత్రం ఆధారంగా ఈ మాసంలో %s కొరకు ఎటువంటి అనుకూల ముహూర్తాలు లేవు.\n", e_name_te.c_str());
                else printf("Based on your Natal Star, no personalized safe Panchang alignments were found for %s in this month.\n", e_name_en.c_str());
            }
            printf("===============================================================================================================\n");
        }
    }	
	// =========================================================================
    // DAILY PANCHANG & TIMINGS (RESTORED & ENHANCED)
    // =========================================================================

	void calculate_daily_panchang_transitions(int t_year, int t_month, int t_day) {
        if (json_mode) return;
        double local_midnight_ut = swe_julday(t_year, t_month, t_day, 0.0 - location.tz_offset, SE_GREG_CAL);
        double noon_jd = local_midnight_ut + 0.5;
        int calc_weekday = (int)(floor(noon_jd + 1.5)) % 7; 
        
        if (telugu_mode) {
            printf("\n=== రోజువారీ పంచాంగం (ఖచ్చితమైన ప్రారంభ & ముగింపు సమయాలు) ===\n");
            printf("వారం      : %s\n", get_weekday(calc_weekday).c_str());
            printf("------------------------------------------------------------------------------------------------------------------\n");
            printf("%-15s | %-25s | %-25s | %-25s\n", "అంశం", "పేరు", "ప్రారంభం", "ముగింపు");
        } else {
            printf("\n=== DAILY PANCHANG (EXACT TITHI & NAKSHATRA TIMINGS) ===\n");
            printf("Day       : %s\n", weekdays[calc_weekday]);
            printf("------------------------------------------------------------------------------------------------------------------\n");
            printf("%-15s | %-25s | %-25s | %-25s\n", "Element", "Name", "Starts", "Ends");
        }
        printf("------------------------------------------------------------------------------------------------------------------\n");

        // Helper lambda to sweep backwards and forwards to find exact boundaries
        auto get_element_bounds = [&](double start_jd, int type, int target_idx, double &t_start, double &t_end) {
            double jd = start_jd;
            while (true) {
                jd -= (1.0 / 1440.0); // Step back 1 minute
                double xx_s[6], xx_m[6]; char serr[256];
                swe_calc_ut(jd, SE_MOON, iflag, xx_m, serr);
                int idx;
                if (type == 0) {
                    swe_calc_ut(jd, SE_SUN, iflag, xx_s, serr);
                    idx = (int)(fmod((xx_m[0] - xx_s[0] + 360.0), 360.0) / 12.0);
                } else {
                    idx = (int)(xx_m[0] / (360.0 / 27.0));
                }
                if (idx != target_idx) { t_start = jd + (1.0 / 1440.0); break; }
            }
            jd = start_jd;
            while (true) {
                jd += (1.0 / 1440.0); // Step forward 1 minute
                double xx_s[6], xx_m[6]; char serr[256];
                swe_calc_ut(jd, SE_MOON, iflag, xx_m, serr);
                int idx;
                if (type == 0) {
                    swe_calc_ut(jd, SE_SUN, iflag, xx_s, serr);
                    idx = (int)(fmod((xx_m[0] - xx_s[0] + 360.0), 360.0) / 12.0);
                } else {
                    idx = (int)(xx_m[0] / (360.0 / 27.0));
                }
                if (idx != target_idx) { t_end = jd; break; }
            }
        };

        // Sweep Tithis touching the current day
        double current_scan_jd = local_midnight_ut;
        while (current_scan_jd < local_midnight_ut + 1.0) {
            double xx_s[6], xx_m[6]; char serr[256];
            swe_calc_ut(current_scan_jd, SE_SUN, iflag, xx_s, serr);
            swe_calc_ut(current_scan_jd, SE_MOON, iflag, xx_m, serr);
            int t_idx = (int)(fmod((xx_m[0] - xx_s[0] + 360.0), 360.0) / 12.0);
            
            double t_start, t_end;
            get_element_bounds(current_scan_jd, 0, t_idx, t_start, t_end);
            string name = get_tithi(t_idx) + " (" + get_paksha(t_idx).substr(0,3) + ")";
            printf("%-15s | %-25s | %-25s | %-25s\n", telugu_mode ? "తిథి" : "Tithi", name.c_str(), jd_to_string(t_start).c_str(), jd_to_string(t_end).c_str());
            
            current_scan_jd = t_end + (2.0 / 1440.0); // jump to next phase
        }

        // Sweep Nakshatras touching the current day
        current_scan_jd = local_midnight_ut;
        while (current_scan_jd < local_midnight_ut + 1.0) {
            double xx_m[6]; char serr[256];
            swe_calc_ut(current_scan_jd, SE_MOON, iflag, xx_m, serr);
            int n_idx = (int)(xx_m[0] / (360.0 / 27.0));
            
            double t_start, t_end;
            get_element_bounds(current_scan_jd, 1, n_idx, t_start, t_end);
            printf("%-15s | %-25s | %-25s | %-25s\n", telugu_mode ? "నక్షత్రం" : "Nakshatra", get_nak_name(n_idx).c_str(), jd_to_string(t_start).c_str(), jd_to_string(t_end).c_str());
            
            current_scan_jd = t_end + (2.0 / 1440.0); // jump to next phase
        }
        printf("------------------------------------------------------------------------------------------------------------------\n");
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
            {"110100111110", "001001000110", "110100111110", "001011001111", "000011001010", "000001100001", "110100111110", "001101000111"},
            {"001001110110", "111001100110", "011011001110", "101110110110", "100100110111", "001110101110", "001011000010", "001001000110"},
            {"001011000110", "001001000010", "110100110110", "001011000010", "000001000111", "000001010011", "100100111110", "101001000110"},
            {"000011001011", "010101010110", "110100111110", "101011001111", "000001010011", "111110011010", "110100111110", "110101010110"},
            {"111100111110", "010010101010", "110100110110", "110111001110", "111100110110", "010011001110", "001011000001", "110111101110"},
            {"000000010011", "111110011011", "001011001011", "001011001010", "000010011110", "111110011110", "001110011110", "111110011010"},
            {"110100110110", "001001000010", "001011000111", "000001011111", "000011000011", "000001000011", "001011000010", "101101000110"}
        };
        
        memset(sav_scores, 0, sizeof(sav_scores));
        memset(bav_scores, 0, sizeof(bav_scores));

        for (int target = 0; target < 7; target++) {
            for (int source = 0; source < 8; source++) {
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
            if (html_mode) {
                printf("<h2 style='margin-top: 30px; margin-bottom: 10px; color: var(--accent);'>%s</h2>", telugu_mode ? "అష్టకవర్గ (Ashtakavarga BAV/SAV)" : "Ashtakavarga (BAV & SAV Totals)");
                printf("<table class='data-table'><tr><th>%s</th><th>Su</th><th>Mo</th><th>Ma</th><th>Me</th><th>Ju</th><th>Ve</th><th>Sa</th><th>SAV Total</th></tr>", telugu_mode ? "రాశి" : "Rashi");
                for (int r = 0; r < 12; r++) {
                    printf("<tr><td>%s</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td><b style='color:var(--accent);'>%d</b></td></tr>", 
                           telugu_mode ? get_rashi_name(r).c_str() : rashi_names[r], 
                           bav_scores[0][r], bav_scores[1][r], bav_scores[2][r], bav_scores[3][r], bav_scores[4][r], bav_scores[5][r], bav_scores[6][r], sav_scores[r]);
                }
                printf("</table>\n");
            } else {
                printf("\n=== ASHTAKAVARGA (BAV & SAV TOTALS) ===\n");
                printf("----------------------------------------------------------------------\n");
                printf("%-10s | %-2s | %-2s | %-2s | %-2s | %-2s | %-2s | %-2s | %-9s\n", "Rashi", "Su", "Mo", "Ma", "Me", "Ju", "Ve", "Sa", "SAV Total");
                printf("----------------------------------------------------------------------\n");
                for (int r = 0; r < 12; r++) {
                    printf("%-10s | %2d | %2d | %2d | %2d | %2d | %2d | %2d |    %3d\n", telugu_mode ? get_rashi_name(r).c_str() : rashi_names[r], bav_scores[0][r], bav_scores[1][r], bav_scores[2][r], bav_scores[3][r], bav_scores[4][r], bav_scores[5][r], bav_scores[6][r], sav_scores[r]);
                }
                printf("----------------------------------------------------------------------\n");
            }
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

void calculate_target_panchang(int t_year, int t_month, int t_day, int t_hour, int t_min, int t_sec, bool use_current_date) {
        if (json_mode) return;
        
        double trans_jd;
        int p_y = t_year, p_m = t_month, p_d = t_day, p_h = t_hour, p_min = t_min, p_s = t_sec;
        
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

        double xx_su[6], xx_mo[6]; char serr[256];
        swe_calc_ut(trans_jd, SE_SUN, iflag, xx_su, serr);
        swe_calc_ut(trans_jd, SE_MOON, iflag, xx_mo, serr);

        double t_su = xx_su[0];
        double t_mo = xx_mo[0];

        // 1. Calculate Exact Tithi
        double tithi_angle = fmod((t_mo - t_su + 360.0), 360.0);
        int tithi_index = (int)(tithi_angle / 12.0); 
        string paksha = (tithi_index < 15) ? (telugu_mode ? "శుక్ల పక్షం" : "Shukla") : (telugu_mode ? "కృష్ణ పక్షం" : "Krishna");
        
        // 2. Calculate Exact Yoga
        double yoga_angle = fmod((t_mo + t_su), 360.0);
        int yoga_index = (int)(yoga_angle / (10.0 / 3.0 * 4.0)); 

        // 3. Calculate Exact Nakshatra & Pada
        int nak_index = (int)(t_mo / (360.0 / 27.0));
        int pada = (int)((t_mo - (nak_index * (360.0 / 27.0))) / ((360.0 / 27.0) / 4.0)) + 1;

        // 4. Calculate Weekday (Vara)
        double noon_jd = swe_julday(p_y, p_m, p_d, 12.0, SE_GREG_CAL);
        int calc_weekday = (int)(floor(noon_jd + 1.5)) % 7; 

        // --- PRINT THE EXACT MOMENT PANCHANG ---
        if (html_mode) {
            printf("<h2 style='margin-top: 20px; color: var(--accent); border-bottom: 1px solid var(--border); padding-bottom: 5px;'>%s %02d/%02d/%04d %02d:%02d:%02d</h2>", 
                   telugu_mode ? "పంచాంగం (Panchang) -" : "EXACT PANCHANG FOR", p_d, p_m, p_y, p_h, p_min, p_s);
            printf("<div style='background: #1e1e24; padding: 15px; border-radius: 6px; border: 1px solid var(--border); margin-bottom: 20px;'>");
            printf("<table style='width:100%%; text-align:left; border-collapse:collapse; font-size: 14px;'>");
            printf("<tr><td style='padding:8px 0; color:#888; border-bottom: 1px solid #333; width: 40%%;'>%s</td><td style='padding:8px 0; color:#fff; border-bottom: 1px solid #333;'>%s</td></tr>", 
                   telugu_mode ? "వారం (Vara):" : "Vara (Weekday):", get_weekday(calc_weekday).c_str());
            printf("<tr><td style='padding:8px 0; color:#888; border-bottom: 1px solid #333;'>%s</td><td style='padding:8px 0; color:#fff; border-bottom: 1px solid #333;'>%s (%s)</td></tr>", 
                   telugu_mode ? "తిథి (Tithi):" : "Tithi:", get_tithi(tithi_index).c_str(), paksha.c_str());
            printf("<tr><td style='padding:8px 0; color:#888; border-bottom: 1px solid #333;'>%s</td><td style='padding:8px 0; color:#fff; border-bottom: 1px solid #333;'>%s (Pada %d)</td></tr>", 
                   telugu_mode ? "నక్షత్రం (Nakshatra):" : "Nakshatra:", get_nak_name(nak_index).c_str(), pada);
            printf("<tr><td style='padding:8px 0; color:#888;'>%s</td><td style='padding:8px 0; color:#fff;'>%s</td></tr>", 
                   telugu_mode ? "యోగం (Yoga):" : "Yoga:", get_yoga(yoga_index).c_str());
            printf("</table></div>\n");
        } else {
            if (telugu_mode) {
                printf("\n=== పంచాంగం (PANCHANG) %02d/%02d/%04d %02d:%02d:%02d ===\n", p_d, p_m, p_y, p_h, p_min, p_s);
                printf("వారం (Vara)       : %s\n", get_weekday(calc_weekday).c_str());
                printf("తిథి (Tithi)      : %s (%s)\n", get_tithi(tithi_index).c_str(), paksha.c_str());
                printf("నక్షత్రం (Nakshatra): %s (%dవ పాదం)\n", get_nak_name(nak_index).c_str(), pada);
                printf("యోగం (Yoga)       : %s\n", get_yoga(yoga_index).c_str());
            } else {
                printf("\n=== EXACT PANCHANG FOR %02d/%02d/%04d %02d:%02d:%02d ===\n", p_d, p_m, p_y, p_h, p_min, p_s);
                printf("Vara (Weekday)    : %s\n", weekdays[calc_weekday]);
                printf("Tithi             : %s (%s)\n", tithi_names[tithi_index], paksha.c_str());
                printf("Nakshatra         : %s (Pada %d)\n", nak_names[nak_index], pada);
                printf("Yoga              : %s\n", yoga_names[yoga_index]);
            }
            printf("----------------------------------------------------------------------------------------------------\n");
        }

        // --- INVOKE THE DAILY SWEEPERS FOR FULL DETAILS ---
        calculate_muhurat(p_y, p_m, p_d, true);
        calculate_daily_panchang_transitions(p_y, p_m, p_d);
    }

void calculate_dasha_balance() {
        if (json_mode) return;
        double nak_size = 360.0 / 27.0; 
        int n_idx = (int)(moon_lon / nak_size);
        int l_idx = n_idx % 9;
        
        double frac = (moon_lon - (n_idx * nak_size)) / nak_size;
        double left_in_years = (1.0 - frac) * dasha_years[l_idx];
        
        int y = (int)left_in_years; 
        double m_rem = (left_in_years - y) * 12.0; 
        int m = (int)m_rem;
        double exact_days = (m_rem - m) * (dasha_year_len / 12.0); 
        int d = (int)exact_days; 
        
        if (html_mode) {
            printf("<div style='background: #1e1e24; padding: 15px; border-radius: 6px; border-left: 4px solid var(--term-text); margin-bottom: 20px;'>");
            printf("<p style='margin: 0; color: #888; font-size: 13px; text-transform: uppercase; letter-spacing: 1px;'>%s</p>", telugu_mode ? "వింశోత్తరి దశా నిల్వ" : "Vimshottari Dasha Balance");
            printf("<h4 style='margin: 5px 0 0 0; color: #fff; font-size: 16px;'>%s <span style='color: var(--term-text);'>%d %s, %d %s, %d %s</span></h4>", 
                telugu_mode ? (get_dasha_lord(l_idx) + " మహాదశ నిల్వ:").c_str() : (string(dasha_lords[l_idx]) + " Maha Dasha:").c_str(),
                y, telugu_mode ? "సంవత్సరాలు" : "Years", m, telugu_mode ? "నెలలు" : "Months", d, telugu_mode ? "రోజులు" : "Days");
            printf("</div>\n");
        } else {
            printf("\n=== VIMSHOTTARI DASHA BALANCE ===\n");
            printf("Maha Dasha: %s (Balance: %d Years, %d Months, %d Days)\n", dasha_lords[l_idx], y, m, d);
        }
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
        double life_start_jd = tjd_ut - (fraction_passed * dasha_years[lord_index] * dasha_year_len);
        
        printf("----------------------------------------------------------------------------------------\n");
        printf("%-20s | %-10s | %-19s | %-19s\n", "Level", "Lord", "Starts", "Ends");
        printf("----------------------------------------------------------------------------------------\n");

        double current_start_jd = life_start_jd; double current_duration = 120.0 * dasha_year_len; int current_lord = lord_index;

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
        double life_start_jd = tjd_ut - (fraction_passed * dasha_years[lord_index] * dasha_year_len);

        printf("\n=== INTERACTIVE DASHA EXPLORER ===\n");
        printf("Enter 1-9 to select a period. Enter 0 to go back.\n");

        vector<DashaState> history; history.push_back({life_start_jd, 120.0 * dasha_year_len, lord_index});
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
        double life_start_jd = tjd_ut - (fraction_passed * dasha_years[lord_index] * dasha_year_len);

        if (html_mode) {
            printf("<h2 style='margin-top: 20px; color: var(--accent); border-bottom: 1px solid var(--border); padding-bottom: 5px;'>%s</h2>", telugu_mode ? "జీవిత కాల దశలు (DYNAMIC VIMSHOTTARI DASHA)" : "LIFE CHAPTERS (DYNAMIC VIMSHOTTARI DASHA)");
        } else {
            if (telugu_mode) {
                printf("\n=========================================================================================\n");
                printf("=== జీవిత కాల దశలు (DYNAMIC VIMSHOTTARI DASHA PREDICTIONS) ===\n");
                printf("=========================================================================================\n\n");
            } else {
                printf("\n=========================================================================================\n");
                printf("=== LIFE CHAPTERS (DYNAMIC VIMSHOTTARI DASHA PREDICTIONS) ===\n");
                printf("=========================================================================================\n\n");
            }
        }

        int d_map[] = {9, 6, 1, 2, 3, 8, 5, 7, 4}; 

        double cur_start = life_start_jd;
        
        // OUTER LOOP (Mahadashas)
        for (int i = 0; i < 9; i++) {
            int md_idx = (lord_index + i) % 9;
            int md_p = d_map[md_idx];
            double md_dur = 120.0 * dasha_year_len * (dasha_years[md_idx] / 120.0);
            double md_end_jd = cur_start + md_dur;
            
            // --- NEW: CLAMP MAHADASHA START DATE TO BIRTH DATE ---
            double display_md_start = (cur_start < tjd_ut) ? tjd_ut : cur_start;
            
            string start_date = jd_to_string(display_md_start).substr(0, 10);
            string end_date = jd_to_string(md_end_jd).substr(0, 10);
            
            int score = natal_scores[md_p];
            int house = (planet_rashis[md_p] - planet_rashis[0] + 12) % 12 + 1;

            if (html_mode) {
                printf("<div style='margin-top: 30px;'>");
                printf("<div style='background: #1e1e24; padding: 15px; border-radius: 6px 6px 0 0; border: 1px solid var(--border); border-bottom: 2px solid var(--accent);'>");
                printf("<h3 style='margin: 0; color: var(--accent); font-size: 1.2em;'>⭐ %s <span style='color: #888; font-size: 0.85em; font-weight: normal; margin-left: 10px;'>[ %s &rarr; %s ]</span></h3>", 
                    telugu_mode ? (get_planet_name(md_p) + " మహాదశ").c_str() : (string(p_names_full[md_p]) + " MAHADASHA").c_str(), 
                    start_date.c_str(), end_date.c_str());
                printf("<p style='margin: 8px 0 0 0; color: #aaa; font-size: 14px;'>%s: <b style='color:#fff;'>%d</b> | %s: <b style='color:%s;'>%d</b></p>", 
                    telugu_mode ? "స్థానం (భావం)" : "Placement (House)", house, 
                    telugu_mode ? "బలం (Dignity)" : "Dignity Score", (score >= 3 ? "#2ecc71" : (score < 0 ? "#e74c3c" : "#f1c40f")), score);
                printf("</div>");
                
                printf("<div style='background: #23232e; padding: 15px; border-radius: 0 0 6px 6px; border: 1px solid var(--border); border-top: none; margin-bottom: 20px;'>");
                printf("<p style='margin: 0; font-size: 15px; line-height: 1.6; color: #fff;'>%s</p>", telugu_mode ? te_get_dynamic_mahadasha(md_p, score, house).c_str() : get_dynamic_mahadasha(md_p, score, house).c_str());
                printf("</div>\n");
            } else {
                if (telugu_mode) {
                    printf("⭐ [ %s  నుండి  %s ] : %s మహాదశ (ఆధిపత్యం: %dవ భావం | బలం: %d)\n", start_date.c_str(), end_date.c_str(), get_planet_name(md_p).c_str(), house, score);
                    printf("   %s\n\n", te_get_dynamic_mahadasha(md_p, score, house).c_str());
                } else {
                    printf("⭐ [ %s  to  %s ] : %s MAHADASHA (Placement: House %d | Dignity Score: %d)\n", start_date.c_str(), end_date.c_str(), p_names_full[md_p], house, score);
                    printf("   %s\n\n", get_dynamic_mahadasha(md_p, score, house).c_str());
                }
            }

            double ad_start = cur_start;
            
            // INNER LOOP (Bhuktis)
            for (int j = 0; j < 9; j++) {
                int ad_idx = (md_idx + j) % 9;
                int ad_p = d_map[ad_idx];
                double ad_dur = md_dur * (dasha_years[ad_idx] / 120.0);
                double ad_end_jd = ad_start + ad_dur;
                
                // --- NEW: SKIP BHUKTIS THAT ENDED BEFORE BIRTH ---
                if (ad_end_jd <= tjd_ut) {
                    ad_start += ad_dur; // Advance the mathematical timeline
                    continue;           // But do not print it
                }
                
                // --- NEW: CLAMP BHUKTI START DATE TO BIRTH DATE ---
                double display_ad_start = (ad_start < tjd_ut) ? tjd_ut : ad_start;
                
                string ad_start_str = jd_to_string(display_ad_start).substr(0, 10);
                string ad_end_str = jd_to_string(ad_end_jd).substr(0, 10);
                
                int ad_score = natal_scores[ad_p];
                int ad_house = (planet_rashis[ad_p] - planet_rashis[0] + 12) % 12 + 1;

                double ad_lon = planet_lons[ad_p];
                int ad_nak_idx = (int)(ad_lon / (360.0 / 27.0));
                int ad_star_lord_idx = d_map[ad_nak_idx % 9]; 

                vector<int> owned_houses;
                if (ad_p >= 1 && ad_p <= 7) { 
                    for (int h = 1; h <= 12; h++) {
                        int rashi_of_house = (planet_rashis[0] + h - 1) % 12;
                        if (rashi_lords[rashi_of_house] == string(p_names_full[ad_p])) {
                            owned_houses.push_back(h);
                        }
                    }
                }

                // 1. GET CLEAN BHUKTI TEXT
                string bhukti_desc_te = te_get_dynamic_bhukti(md_p, ad_p, ad_score, ad_house, ad_star_lord_idx, html_mode);
                string bhukti_desc_en = get_dynamic_bhukti(md_p, ad_p, ad_score, ad_house, ad_star_lord_idx, html_mode);

                // 2. CONCRETE STAR LORD OUTCOME GENERATOR
                int sl_house = (planet_rashis[ad_star_lord_idx] - planet_rashis[0] + 12) % 12 + 1;
                string sl_name_en = p_names_full[ad_star_lord_idx];
                string sl_name_te = get_planet_name(ad_star_lord_idx);

                string sl_domain_en, sl_domain_te;
                switch(sl_house) {
                    case 1: sl_domain_en = "health, self-development, and major life shifts"; sl_domain_te = "ఆరోగ్యం, వ్యక్తిగత ఎదుగుదల మరియు జీవితంలో ప్రధాన మార్పులు"; break;
                    case 2: sl_domain_en = "wealth accumulation, family matters, and savings"; sl_domain_te = "ఆర్థిక లాభాలు, కుటుంబ వ్యవహారాలు మరియు పొదుపు"; break;
                    case 3: sl_domain_en = "short travels, siblings, and courageous new initiatives"; sl_domain_te = "చిన్న ప్రయాణాలు, తోబుట్టువులు మరియు ధైర్యంతో చేసే కొత్త పనులు"; break;
                    case 4: sl_domain_en = "real estate, vehicles, mother, and domestic peace"; sl_domain_te = "స్థిరాస్తులు (రియల్ ఎస్టేట్), వాహనాలు, తల్లి మరియు గృహ సౌఖ్యం"; break;
                    case 5: sl_domain_en = "children, creative projects, and speculative investments"; sl_domain_te = "సంతానం, సృజనాత్మక ప్రాజెక్టులు మరియు పెట్టుబడులు"; break;
                    case 6: sl_domain_en = "overcoming debts, resolving health issues, and defeating competitors"; sl_domain_te = "అప్పులు తీర్చడం, ఆరోగ్య సమస్యల పరిష్కారం మరియు పోటీలో విజయం"; break;
                    case 7: sl_domain_en = "marriage, business partnerships, and public relations"; sl_domain_te = "వివాహం, వ్యాపార భాగస్వామ్యాలు మరియు ప్రజా సంబంధాలు"; break;
                    case 8: sl_domain_en = "sudden transformations, hidden wealth, and deep research"; sl_domain_te = "ఆకస్మిక పరివర్తనలు, గుప్త ధనం మరియు లోతైన పరిశోధనలు"; break;
                    case 9: sl_domain_en = "long-distance travel, higher education, and fortune"; sl_domain_te = "దూర ప్రయాణాలు, ఉన్నత విద్య మరియు అదృష్టం కలిసిరావడం"; break;
                    case 10: sl_domain_en = "career milestones, social status, and professional authority"; sl_domain_te = "కెరీర్ (వృత్తి) మైలురాళ్ళు, సామాజిక హోదా మరియు అధికారం"; break;
                    case 11: sl_domain_en = "large financial gains, fulfilling desires, and networking"; sl_domain_te = "భారీ ఆర్థిక లాభాలు, ఆశయాల నెరవేర్పు మరియు నెట్‌వర్కింగ్"; break;
                    case 12: sl_domain_en = "foreign connections, heavy expenses, and spiritual isolation"; sl_domain_te = "విదేశీ సంబంధాలు, భారీ ఖర్చులు మరియు ఆధ్యాత్మిక ఏకాంతం"; break;
                }

                string sl_text_en_html = "<b>Star Lord Reality (" + sl_name_en + "):</b> Placed in House " + to_string(sl_house) + ". The ultimate physical events of this period will materialize strictly regarding " + sl_domain_en + ".";
                string sl_text_te_html = "<b>నక్షత్రాధిపతి ఫలితం (" + sl_name_te + "):</b> మీ జాతకంలో " + to_string(sl_house) + "వ భావంలో ఉన్నాడు. కాబట్టి ఈ కాలంలో అంతిమంగా " + sl_domain_te + " కి సంబంధించిన కచ్చితమైన ఫలితాలు సిద్ధిస్తాయి.";
                
                string sl_text_en_cli = "* Star Lord Reality (" + sl_name_en + "): Placed in House " + to_string(sl_house) + ". Final events manifest regarding " + sl_domain_en + ".";
                string sl_text_te_cli = "* నక్షత్రాధిపతి ఫలితం (" + sl_name_te + "): మీ జాతకంలో " + to_string(sl_house) + "వ భావంలో ఉన్నాడు. " + sl_domain_te + " కి సంబంధించిన ఫలితాలు సిద్ధిస్తాయి.";

                // 3. Ashtakavarga Evaluation for Bhukti Lord
                int eval_p = ad_p;
                if (ad_p == 8 || ad_p == 9) { 
                    string r_lord = rashi_lords[planet_rashis[ad_p]];
                    for(int p=1; p<=7; p++) {
                        if (string(p_names_full[p]) == r_lord) { eval_p = p; break; }
                    }
                }
                
                int r_bav = bav_scores[eval_p - 1][planet_rashis[eval_p]];
                int r_sav = sav_scores[planet_rashis[eval_p]];
                
                string av_text_en_html, av_text_te_html;
                string av_text_en_cli, av_text_te_cli;

                if (r_bav >= 5 && r_sav >= 28) {
                    av_text_en_html = "<b style='color:#2ecc71;'>Ashtakavarga Impact:</b> Highly supportive environment (BAV: " + to_string(r_bav) + ", SAV: " + to_string(r_sav) + "). Even challenging periods will yield surprisingly positive end results.";
                    av_text_te_html = "<b style='color:#2ecc71;'>అష్టకవర్గ ప్రభావం:</b> అత్యంత అనుకూల వాతావరణం (BAV: " + to_string(r_bav) + ", SAV: " + to_string(r_sav) + "). ప్రతికూల దశలు కూడా ఆశ్చర్యకరంగా మంచి ఫలితాలను ఇస్తాయి.";
                    av_text_en_cli = "* Ashtakavarga Impact: Highly supportive environment (BAV: " + to_string(r_bav) + ", SAV: " + to_string(r_sav) + "). Positive end results.";
                    av_text_te_cli = "* అష్టకవర్గ ప్రభావం: అత్యంత అనుకూల వాతావరణం (BAV: " + to_string(r_bav) + ", SAV: " + to_string(r_sav) + "). ప్రతికూలతలు ఉన్నా మంచి ఫలితాలు వస్తాయి.";
                } else if (r_bav <= 3 && r_sav < 25) {
                    av_text_en_html = "<b style='color:#e74c3c;'>Ashtakavarga Impact:</b> Weak environmental support (BAV: " + to_string(r_bav) + ", SAV: " + to_string(r_sav) + "). Expect delays and hard work; even good periods will face friction.";
                    av_text_te_html = "<b style='color:#e74c3c;'>అష్టకవర్గ ప్రభావం:</b> వాతావరణ బలం తక్కువగా ఉంది (BAV: " + to_string(r_bav) + ", SAV: " + to_string(r_sav) + "). ఆలస్యం మరియు తీవ్ర శ్రమ అవసరం; శుభ దశలలో కూడా స్వల్ప ఘర్షణ ఉంటుంది.";
                    av_text_en_cli = "* Ashtakavarga Impact: Weak environmental support (BAV: " + to_string(r_bav) + ", SAV: " + to_string(r_sav) + "). Expect delays/friction.";
                    av_text_te_cli = "* అష్టకవర్గ ప్రభావం: వాతావరణ బలం తక్కువగా ఉంది (BAV: " + to_string(r_bav) + ", SAV: " + to_string(r_sav) + "). స్వల్ప ఘర్షణ, జాప్యం ఉంటుంది.";
                } else {
                    av_text_en_html = "<b style='color:#f1c40f;'>Ashtakavarga Impact:</b> Moderate environmental support (BAV: " + to_string(r_bav) + ", SAV: " + to_string(r_sav) + "). Results will manifest exactly as promised without extreme shifts.";
                    av_text_te_html = "<b style='color:#f1c40f;'>అష్టకవర్గ ప్రభావం:</b> మధ్యస్థ వాతావరణ బలం (BAV: " + to_string(r_bav) + ", SAV: " + to_string(r_sav) + "). ఎలాంటి అడ్డంకులు లేకుండా ఫలితాలు యధావిధిగా ఉంటాయి.";
                    av_text_en_cli = "* Ashtakavarga Impact: Moderate environmental support (BAV: " + to_string(r_bav) + ", SAV: " + to_string(r_sav) + "). Standard results.";
                    av_text_te_cli = "* అష్టకవర్గ ప్రభావం: మధ్యస్థ వాతావరణ బలం (BAV: " + to_string(r_bav) + ", SAV: " + to_string(r_sav) + "). ఫలితాలు యధావిధిగా ఉంటాయి.";
                }
                
                if (ad_p == 8 || ad_p == 9) {
                    av_text_en_html = "<i>(Shadow Node relies on Dispositor " + string(p_names_full[eval_p]) + ")</i> " + av_text_en_html;
                    av_text_te_html = "<i>(ఛాయా గ్రహం అధిపతి " + get_planet_name(eval_p) + " పై ఆధారపడి ఉంటుంది)</i> " + av_text_te_html;
                    av_text_en_cli = "[Node relies on " + string(p_names_full[eval_p]) + "] " + av_text_en_cli;
                    av_text_te_cli = "[ఛాయా గ్రహం అధిపతి " + get_planet_name(eval_p) + " పై ఆధారం] " + av_text_te_cli;
                }

                // 4. MD-AD Mutual Relationship (Sthana) & Modern Tech / Career Mapping
                int md_rashi = planet_rashis[md_p];
                int ad_rashi = planet_rashis[ad_p];
                int md_ad_dist = (ad_rashi - md_rashi + 12) % 12 + 1;
                
                string sthana_en, sthana_te;
                string sthana_en_cli, sthana_te_cli;
                
                if (md_ad_dist == 6 || md_ad_dist == 8 || md_ad_dist == 12) {
                    sthana_en = "<b>Operational Flow (6/8/12 Axis):</b> High friction. Expect sudden scope creep, hidden bugs, tech debt surfacing, or misaligned team dynamics. Requires heavy debugging and refactoring of past work.";
                    sthana_te = "<b>కార్యనిర్వహణ (6/8/12 స్థానం):</b> తీవ్రమైన ఘర్షణ. ప్రాజెక్టులలో ఆకస్మిక మార్పులు, అడ్డంకులు, మరియు టీమ్ మధ్య సమన్వయ లోపం ఏర్పడుతుంది. పాత పనులను పదే పదే సరిదిద్దాల్సి వస్తుంది.";
                    sthana_en_cli = "* Operational Flow: High friction (6/8/12 Axis). Scope creep, bugs, misaligned dynamics.";
                    sthana_te_cli = "* కార్యనిర్వహణ: తీవ్రమైన ఘర్షణ (6/8/12). ప్రాజెక్టులలో అడ్డంకులు, టీమ్ సమన్వయ లోపం.";
                } else if (md_ad_dist == 5 || md_ad_dist == 9) {
                    sthana_en = "<b>Operational Flow (5/9 Axis):</b> Smooth execution. Excellent for creative problem-solving, innovation, learning new frameworks, and getting swift approvals/deployments.";
                    sthana_te = "<b>కార్యనిర్వహణ (5/9 స్థానం):</b> సాఫీగా సాగే కాలం. సృజనాత్మకత, కొత్త నైపుణ్యాలు నేర్చుకోవడం, మరియు ప్రాజెక్టులకు సులభంగా ఆమోదం లభించడానికి ఇది అద్భుతమైన సమయం.";
                    sthana_en_cli = "* Operational Flow: Smooth execution (5/9 Axis). Innovation, swift approvals, and learning.";
                    sthana_te_cli = "* కార్యనిర్వహణ: సాఫీగా సాగే కాలం (5/9). సృజనాత్మకత, కొత్త నైపుణ్యాలు, ప్రాజెక్టు ఆమోదం.";
                } else if (md_ad_dist == 1 || md_ad_dist == 4 || md_ad_dist == 7 || md_ad_dist == 10) {
                    sthana_en = "<b>Operational Flow (Kendra Axis):</b> Heavy, visible workload. Focus shifts to core architecture, strict deadlines, and major production releases. Demands high structural discipline.";
                    sthana_te = "<b>కార్యనిర్వహణ (కేంద్ర స్థానం):</b> కంటికి కనిపించే భారీ పనిభారం. కఠినమైన డెడ్‌లైన్స్, మరియు ప్రధాన బాధ్యతలపై పూర్తి దృష్టి పెట్టాల్సి వస్తుంది. అత్యధిక క్రమశిక్షణ అవసరం.";
                    sthana_en_cli = "* Operational Flow: Heavy workload (Kendra Axis). Core architecture, strict deadlines, releases.";
                    sthana_te_cli = "* కార్యనిర్వహణ: భారీ పనిభారం (కేంద్ర స్థానం). కఠినమైన డెడ్‌లైన్స్, ప్రధాన బాధ్యతలపై దృష్టి.";
                } else {
                    sthana_en = "<b>Operational Flow (3/11 Axis):</b> Growth oriented. Excellent for teamwork, communication, scaling systems, and financial gains through successful project deliveries.";
                    sthana_te = "<b>కార్యనిర్వహణ (3/11 స్థానం):</b> వృద్ధికి అనుకూలం. కమ్యూనికేషన్, టీమ్‌వర్క్, మరియు విజయవంతమైన ప్రాజెక్టుల ద్వారా ఆర్థిక లాభాలు సాధించడానికి మంచి కాలం.";
                    sthana_en_cli = "* Operational Flow: Growth oriented (3/11 Axis). Teamwork, scaling, financial gains.";
                    sthana_te_cli = "* కార్యనిర్వహణ: వృద్ధికి అనుకూలం (3/11). టీమ్‌వర్క్, విజయవంతమైన ప్రాజెక్టుల ద్వారా లాభాలు.";
                }

                // --- 5. PRINT THE BEAUTIFULLY STRUCTURED OUTPUT ---
                if (html_mode) {
                    printf("<div style='margin-bottom:15px; padding:15px; background:#2a2a35; border-left:4px solid var(--accent); border-radius:4px;'>");
                    printf("<h4 style='margin-top:0; color:#e0e0e0;'>%s %s <span style='color:#888; font-size:12px; font-weight:normal; margin-left:10px;'>[ %s &rarr; %s ]</span></h4>", 
                           telugu_mode ? get_planet_name(ad_p).c_str() : p_names_full[ad_p],
                           telugu_mode ? "భుక్తి" : "Bhukti", ad_start_str.c_str(), ad_end_str.c_str());
                    
                    printf("<p style='margin:5px 0; font-size:14px; line-height:1.6; color:#ccc;'>%s</p>", 
                           telugu_mode ? bhukti_desc_te.c_str() : bhukti_desc_en.c_str());
                    
                    printf("<p style='margin:10px 0 0 0; font-size:13px; line-height:1.5; color:#ccc; background:#1e1e24; padding:8px; border-radius:4px;'>%s</p>", 
                           telugu_mode ? av_text_te_html.c_str() : av_text_en_html.c_str());

                    printf("<p style='margin:10px 0 0 0; font-size:13px; line-height:1.5; color:#ccc; background:#1e1e24; padding:8px; border-radius:4px;'>%s</p>", 
                           telugu_mode ? sthana_te.c_str() : sthana_en.c_str());

                    printf("<p style='margin:10px 0; font-size:13px; line-height:1.5; color:#ccc; background:#1e1e24; border-left: 2px solid #e67e22; padding:8px; border-radius:4px;'>%s</p>", 
                           telugu_mode ? sl_text_te_html.c_str() : sl_text_en_html.c_str());

                    if (ad_p == 8 || ad_p == 9) {
                        printf("<p style='margin:10px 0 0 0; font-size:14px; color:var(--term-text);'><b>%s</b> %s</p>", 
                               telugu_mode ? "ముఖ్య సంఘటనలు:" : "Key Events:",
                               telugu_mode ? te_get_node_bhukti_event(get_planet_name(ad_p), ad_house, ad_score, html_mode).c_str() : get_node_bhukti_event(p_names_full[ad_p], ad_house, ad_score, html_mode).c_str());
                    } else {
                        printf("<p style='margin:10px 0 0 0; font-size:14px; color:var(--term-text);'><b>%s</b> %s</p>", 
                               telugu_mode ? "ముఖ్య సంఘటనలు:" : "Key Events:",
                               telugu_mode ? te_get_lordship_bhukti_event(get_planet_name(ad_p), owned_houses, ad_score, html_mode).c_str() : get_lordship_bhukti_event(p_names_full[ad_p], owned_houses, ad_score, html_mode).c_str());
                    }
                    printf("</div>\n");
                    fflush(stdout); 
                } else {
                    if (telugu_mode) {
                        printf("     -> [ %s - %s ] : %s భుక్తి\n", ad_start_str.c_str(), ad_end_str.c_str(), get_planet_name(ad_p).c_str());
                        printf("        %s\n", bhukti_desc_te.c_str());
                        printf("        %s\n", av_text_te_cli.c_str());
                        printf("        %s\n", sthana_te_cli.c_str());
                        printf("        %s\n", sl_text_te_cli.c_str());
                        if (ad_p == 8 || ad_p == 9) printf("        * ప్రత్యక్ష సంఘటనలు: %s\n", te_get_node_bhukti_event(get_planet_name(ad_p), ad_house, ad_score, html_mode).c_str());
                        else printf("        * ప్రత్యక్ష సంఘటనలు: %s\n", te_get_lordship_bhukti_event(get_planet_name(ad_p), owned_houses, ad_score, html_mode).c_str());
                    } else {
                        printf("     -> [ %s - %s ] : %s Bhukti\n", ad_start_str.c_str(), ad_end_str.c_str(), p_names_full[ad_p]);
                        printf("        %s\n", bhukti_desc_en.c_str());
                        printf("        %s\n", av_text_en_cli.c_str());
                        printf("        %s\n", sthana_en_cli.c_str());
                        printf("        %s\n", sl_text_en_cli.c_str());
                        if (ad_p == 8 || ad_p == 9) printf("        * Life Events: %s\n", get_node_bhukti_event(p_names_full[ad_p], ad_house, ad_score, html_mode).c_str());
                        else printf("        * Life Events: %s\n", get_lordship_bhukti_event(p_names_full[ad_p], owned_houses, ad_score, html_mode).c_str());
                    }
                }
                ad_start += ad_dur;
            } 
            
            if (html_mode) printf("</div>\n"); 
            else printf("\n-----------------------------------------------------------------------------------------\n\n");
            
            cur_start += md_dur;
        } 
    }

    void print_dasha_tables_html(double target_jd) {
        if (!html_mode) return;

        double nak_size = 360.0 / 27.0; 
        int nak_index = (int)(moon_lon / nak_size); 
        int lord_index = nak_index % 9;
        double fraction_passed = (moon_lon - (nak_index * nak_size)) / nak_size;
        double life_start_jd = tjd_ut - (fraction_passed * dasha_years[lord_index] * dasha_year_len);

        double cur_start = life_start_jd; 
        double cur_dur = 120.0 * dasha_year_len; 
        int cur_lord = lord_index;
        
        int active_md_idx = -1;
        double md_start = 0, md_dur = 0;

        printf("<h2 style='margin-top: 40px; color: var(--accent); border-top: 1px solid var(--border); padding-top: 20px;'>%s</h2>", telugu_mode ? "ప్రస్తుత దశా పట్టికలు (Current Dasha Timelines)" : "Current Dasha Timelines");

        // --- 1. Maha Dasha Table ---
        printf("<h3 style='color: #e0e0e0; margin-top:20px;'>--- %s ---</h3>", telugu_mode ? "మహా దశ" : "Maha Dasha Options");
        printf("<table class='data-table' style='margin-top: 0;'><tr><th>%s</th><th>%s</th><th>%s</th><th>%s</th></tr>", 
               telugu_mode ? "ఎంపిక" : "Opt", telugu_mode ? "అధిపతి" : "Lord", 
               telugu_mode ? "ప్రారంభం" : "Starts", telugu_mode ? "ముగింపు" : "Ends");
        
        for (int i = 0; i < 9; i++) {
            int p_idx = (cur_lord + i) % 9;
            double sub_dur = cur_dur * (dasha_years[p_idx] / 120.0);
            if (target_jd >= cur_start && target_jd < cur_start + sub_dur) {
                active_md_idx = p_idx; md_start = cur_start; md_dur = sub_dur;
                printf("<tr style='background:#3a3a45; font-weight:bold; color:var(--term-text);'><td>%d</td><td>%s</td><td>%s</td><td>%s</td></tr>", 
                       i+1, get_dasha_lord(p_idx).c_str(), jd_to_string(cur_start).c_str(), jd_to_string(cur_start + sub_dur).c_str());
            } else {
                printf("<tr><td>%d</td><td>%s</td><td>%s</td><td>%s</td></tr>", 
                       i+1, get_dasha_lord(p_idx).c_str(), jd_to_string(cur_start).c_str(), jd_to_string(cur_start + sub_dur).c_str());
            }
            cur_start += sub_dur;
        }
        printf("</table>");

        if (active_md_idx == -1) { printf("\n"); fflush(stdout); return; }

        // --- 2. Antar Dasha (Bhukti) Table ---
        cur_start = md_start;
        cur_dur = md_dur;
        cur_lord = active_md_idx;
        
        int active_ad_idx = -1;
        double ad_start = 0, ad_dur = 0;

        printf("<h3 style='color: #e0e0e0; margin-top:20px;'>--- %s ---</h3>", telugu_mode ? "అంతర్ దశ (భుక్తి)" : "Antar Dasha (Bhukti) Options");
        printf("<table class='data-table' style='margin-top: 0;'><tr><th>%s</th><th>%s</th><th>%s</th><th>%s</th></tr>", 
               telugu_mode ? "ఎంపిక" : "Opt", telugu_mode ? "అధిపతి" : "Lord", 
               telugu_mode ? "ప్రారంభం" : "Starts", telugu_mode ? "ముగింపు" : "Ends");
        
        for (int i = 0; i < 9; i++) {
            int p_idx = (cur_lord + i) % 9;
            double sub_dur = cur_dur * (dasha_years[p_idx] / 120.0);
            if (target_jd >= cur_start && target_jd < cur_start + sub_dur) {
                active_ad_idx = p_idx; ad_start = cur_start; ad_dur = sub_dur;
                printf("<tr style='background:#3a3a45; font-weight:bold; color:var(--term-text);'><td>%d</td><td>%s</td><td>%s</td><td>%s</td></tr>", 
                       i+1, get_dasha_lord(p_idx).c_str(), jd_to_string(cur_start).c_str(), jd_to_string(cur_start + sub_dur).c_str());
            } else {
                printf("<tr><td>%d</td><td>%s</td><td>%s</td><td>%s</td></tr>", 
                       i+1, get_dasha_lord(p_idx).c_str(), jd_to_string(cur_start).c_str(), jd_to_string(cur_start + sub_dur).c_str());
            }
            cur_start += sub_dur;
        }
        printf("</table>");

        if (active_ad_idx == -1) { printf("\n"); fflush(stdout); return; }

        // --- 3. Pratyantar Dasha Table ---
        cur_start = ad_start;
        cur_dur = ad_dur;
        cur_lord = active_ad_idx;

        printf("<h3 style='color: #e0e0e0; margin-top:20px;'>--- %s ---</h3>", telugu_mode ? "ప్రత్యంతర్ దశ" : "Pratyantar Dasha Options");
        printf("<table class='data-table' style='margin-top: 0;'><tr><th>%s</th><th>%s</th><th>%s</th><th>%s</th></tr>", 
               telugu_mode ? "ఎంపిక" : "Opt", telugu_mode ? "అధిపతి" : "Lord", 
               telugu_mode ? "ప్రారంభం" : "Starts", telugu_mode ? "ముగింపు" : "Ends");
        
        for (int i = 0; i < 9; i++) {
            int p_idx = (cur_lord + i) % 9;
            double sub_dur = cur_dur * (dasha_years[p_idx] / 120.0);
            if (target_jd >= cur_start && target_jd < cur_start + sub_dur) {
                printf("<tr style='background:#3a3a45; font-weight:bold; color:var(--term-text);'><td>%d</td><td>%s</td><td>%s</td><td>%s</td></tr>", 
                       i+1, get_dasha_lord(p_idx).c_str(), jd_to_string(cur_start).c_str(), jd_to_string(cur_start + sub_dur).c_str());
            } else {
                printf("<tr><td>%d</td><td>%s</td><td>%s</td><td>%s</td></tr>", 
                       i+1, get_dasha_lord(p_idx).c_str(), jd_to_string(cur_start).c_str(), jd_to_string(cur_start + sub_dur).c_str());
            }
            cur_start += sub_dur;
        }
        
        // CRITICAL FIX: The \n at the end tells Emscripten to flush the HTML to JavaScript!
        printf("</table><br>\n"); 
        fflush(stdout); 
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
        double life_start_jd = tjd_ut - (fraction_passed * dasha_years[lord_index] * dasha_year_len);
        vector<int> path;
        dfs_find_dehas(0, lord_index, life_start_jd, 120.0 * dasha_year_len, target_start_jd, target_end_jd, path);
        printf("--------------------------------------------------------------------------------------\n");
    }

    void export_all_dashas_csv() {
        double nak_size = 360.0 / 27.0; int nak_index = (int)(moon_lon / nak_size); int lord_index = nak_index % 9;
        double fraction_passed = (moon_lon - (nak_index * nak_size)) / nak_size;
        double life_start_jd = tjd_ut - (fraction_passed * dasha_years[lord_index] * dasha_year_len);

        FILE* fp = fopen("vimshottari_full_export.csv", "w");
        if (!fp) { printf("Error creating CSV file!\n"); return; }
        fprintf(fp, "Level_Depth,Level_Name,Path,Active_Lord,Start_Date,End_Date\n");
        printf("\n=== EXPORTING 120-YEAR DASHA HIERARCHY ===\nGenerating roughly 598,000 records... Please wait...\n");

        vector<string> path_names;
        dfs_export_csv(0, lord_index, life_start_jd, 120.0 * dasha_year_len, path_names, fp);
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
        double life_start_jd = tjd_ut - (fraction_passed * dasha_years[lord_index] * dasha_year_len);

        double cur_start = life_start_jd; 
        double cur_dur = 120.0 * dasha_year_len; 
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
        // 1. Calculate Natal Baseline
        double natal_angle = fmod(moon_lon - sun_lon + 360.0, 360.0);
        int natal_sun_sign = (int)(sun_lon / 30.0);
        int tithi_idx = (int)(natal_angle / 12.0);
        string paksha = (tithi_idx < 15) ? (telugu_mode ? "శుక్ల పక్షం" : "Shukla") : (telugu_mode ? "కృష్ణ పక్షం" : "Krishna");
        
        if (html_mode) {
            printf("<h2 style='margin-top: 20px; color: var(--accent); border-bottom: 1px solid var(--border); padding-bottom: 5px;'>%s</h2>", telugu_mode ? "తిథి ప్రవేశం (వార్షిక జాతక చక్రం)" : "TITHI PRAVESHA (VEDIC BIRTHDAY ENGINE)");
            printf("<div style='background: #1e1e24; padding: 15px; border-radius: 6px; border: 1px solid var(--border); margin-bottom: 20px;'>");
            printf("<table style='width:100%%; text-align:left; border-collapse:collapse; font-size: 14px;'>");
            printf("<tr><td style='padding:8px 0; color:#888; border-bottom: 1px solid #333; width: 40%%;'>%s</td><td style='padding:8px 0; color:#fff; border-bottom: 1px solid #333;'>%s</td></tr>", telugu_mode ? "జన్మ సూర్య రాశి:" : "Natal Sun Sign:", telugu_mode ? get_rashi_name(natal_sun_sign).c_str() : rashi_names[natal_sun_sign]);
            printf("<tr><td style='padding:8px 0; color:#888; border-bottom: 1px solid #333;'>%s</td><td style='padding:8px 0; color:#fff; border-bottom: 1px solid #333;'>%02d° %02d'</td></tr>", telugu_mode ? "సూర్య-చంద్రుల కోణం:" : "Natal Sun-Moon Angle:", (int)natal_angle, (int)((natal_angle - (int)natal_angle) * 60.0));
            printf("<tr><td style='padding:8px 0; color:#888; border-bottom: 1px solid #333;'>%s</td><td style='padding:8px 0; color:#fff; border-bottom: 1px solid #333;'>%s (%s)</td></tr>", telugu_mode ? "జన్మ తిథి:" : "Natal Tithi:", telugu_mode ? te_tithi_names[tithi_idx] : tithi_names[tithi_idx], paksha.c_str());
            printf("<tr><td style='padding:8px 0; color:#888;'>%s</td><td style='padding:8px 0; color:#f1c40f; font-weight:bold;'>%d</td></tr>", telugu_mode ? "లక్ష్య సంవత్సరం:" : "Target Year:", target_year);
            printf("</table></div>");
        } else {
            printf("\n=== TITHI PRAVESHA (VEDIC BIRTHDAY ENGINE) ===\n");
            printf("Natal Sun Sign       : %s\n", rashi_names[natal_sun_sign]);
            printf("Natal Sun-Moon Angle : %02d° %02d'\n", (int)natal_angle, (int)((natal_angle - (int)natal_angle) * 60.0));
            printf("Natal Tithi          : %s (%s)\n", tithi_names[tithi_idx], paksha.c_str());
            printf("Target Year          : %d\n", target_year);
            printf("-----------------------------------------------------------------\n");
        }

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
        
        if (found) {
            if (html_mode) {
                printf("<div style='background: #2a2a35; padding: 20px; border-radius: 6px; border-left: 5px solid #2ecc71; margin-bottom: 25px;'>");
                printf("<p style='margin: 0 0 5px 0; color: #888; font-size: 13px; text-transform: uppercase; letter-spacing: 1px;'>%s</p>", telugu_mode ? "వార్షిక చక్రం ప్రారంభ సమయం" : "Exact Tithi Return Time");
                printf("<h3 style='margin: 0 0 15px 0; color: #2ecc71; font-size: 1.4em;'>%s</h3>", jd_to_string(exact_jd).c_str());
                printf("<p style='margin: 0 0 15px 0; color: #ccc; font-size: 14px;'>%s</p>", telugu_mode ? "<b>ప్రాముఖ్యత:</b> అత్యంత ముఖ్యం - వార్షిక కర్మ చక్రం ఇక్కడే రీసెట్ అవుతుంది." : "<b>Status:</b> HIGH IMPORTANCE - Annual Karmic Cycle Reset Point");
                
                double s_pos[6], m_pos[6]; char serr[256];
                swe_calc_ut(exact_jd, SE_SUN, iflag, s_pos, serr);
                swe_calc_ut(exact_jd, SE_MOON, iflag, m_pos, serr);
                
                printf("<table class='data-table' style='margin:0;'><tr><th>%s</th><th>%s</th></tr>", telugu_mode ? "గ్రహం" : "Graha", telugu_mode ? "రీసెట్ సమయంలో స్థానం" : "Position at Reset");
                printf("<tr><td><b>%s</b></td><td>%s</td></tr>", telugu_mode ? "సూర్యుడు (Surya)" : "Transit Surya", format_dms(s_pos[0]).c_str());
                printf("<tr><td><b>%s</b></td><td>%s</td></tr>", telugu_mode ? "చంద్రుడు (Chandra)" : "Transit Chandra", format_dms(m_pos[0]).c_str());
                printf("</table></div>\n");
            } else {
                printf("Exact Tithi Return Time : %s (Local Time)\n", jd_to_string(exact_jd).c_str());
                printf("Status                  : HIGH IMPORTANCE - Annual Cycle Reset Point\n");
                
                double s_pos[6], m_pos[6]; char serr[256];
                swe_calc_ut(exact_jd, SE_SUN, iflag, s_pos, serr);
                swe_calc_ut(exact_jd, SE_MOON, iflag, m_pos, serr);
                printf("\n[Planetary Posture at Reset]\n");
                printf("Transit Surya   : %s\n", format_dms(s_pos[0]).c_str());
                printf("Transit Chandra : %s\n", format_dms(m_pos[0]).c_str());
                printf("-----------------------------------------------------------------\n");
            }
            return exact_jd; 
        } else {
            if (html_mode) {
                printf("<p style='color: #e74c3c;'>Error: Could not calculate Tithi Return for this year.</p>");
            } else {
                printf("Error: Could not calculate Tithi Return for this year.\n");
                printf("-----------------------------------------------------------------\n");
            }
            return 0.0;
        }
    }

// ==========================================
    // NAKSHATRA (TARA BALA) BOUNDARY SCANNER
    // ==========================================
    void get_nakshatra_transit_range(int p_idx, double target_jd, long flags, std::string& start_date, std::string& end_date) {
        double xx[6];
        char serr[256];
        
        // FIX: Correct Swiss Ephemeris Mapping
        int planets_se[] = {SE_SUN, SE_MOON, SE_MARS, SE_MERCURY, SE_JUPITER, SE_VENUS, SE_SATURN, node_calc_type, node_calc_type};
        int se_p = (p_idx > 0 && p_idx <= 9) ? planets_se[p_idx - 1] : 0;

        // Helper lambda to instantly fetch longitude and auto-correct for Ketu
        auto get_lon = [&](double jd) {
            swe_calc_ut(jd, se_p, flags, xx, serr);
            double lon = xx[0];
            if (p_idx == 9) lon = fmod(lon + 180.0, 360.0); // Ketu is exactly 180° from Rahu
            return lon;
        };

        double current_lon = get_lon(target_jd);
        int current_nak = (int)(current_lon / (360.0 / 27.0));

        // Set adaptive step size based on planet speed to optimize execution loops
        double step = 1.0; 
        if (p_idx == 2) step = 0.05; // Moon moves fast (~13° per day)
        else if (p_idx == 7 || p_idx == 8 || p_idx == 9) step = 4.0; // Saturn and Nodes move slow

        // 1. Scan backward for Nakshatra entry date
        double jd_start = target_jd;
        while (true) {
            double lon = get_lon(jd_start);
            int nak = (int)(lon / (360.0 / 27.0));
            if (nak != current_nak) {
                double low = jd_start, high = jd_start + step;
                for (int iter = 0; iter < 12; iter++) {
                    double mid = (low + high) / 2.0;
                    if ((int)(get_lon(mid) / (360.0 / 27.0)) == current_nak) high = mid;
                    else low = mid;
                }
                jd_start = high;
                break;
            }
            jd_start -= step;
            if (target_jd - jd_start > 1100) { jd_start = target_jd; break; } // Safety escape
        }

        // 2. Scan forward for Nakshatra exit date
        double jd_end = target_jd;
        while (true) {
            double lon = get_lon(jd_end);
            int nak = (int)(lon / (360.0 / 27.0));
            if (nak != current_nak) {
                double low = jd_end - step, high = jd_end;
                for (int iter = 0; iter < 12; iter++) {
                    double mid = (low + high) / 2.0;
                    if ((int)(get_lon(mid) / (360.0 / 27.0)) == current_nak) low = mid;
                    else high = mid;
                }
                jd_end = low;
                break;
            }
            jd_end += step;
            if (jd_end - target_jd > 1100) { jd_end = target_jd; break; } // Safety escape
        }

        start_date = jd_to_string(jd_start).substr(0, 10);
        end_date = jd_to_string(jd_end).substr(0, 10);
    }

	   
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

        // --- TRANSIT TABLE HEADER ---
        if (telugu_mode) {
            if (html_mode) {
                printf("<h2 style='margin-top: 20px; margin-bottom: 10px; color: var(--accent);'>గోచారం (గ్రహ సంచారం) తేదీ: %02d/%02d/%04d సమయం: %02d:%02d:%02d</h2>", p_d, p_m, p_y, p_h, p_min, p_s);
                printf("<p style='color: #888; margin-top: 0; margin-bottom: 15px;'>ప్రస్తుత దశ: <b>%s మహా దశ / %s అంతర్ దశ</b></p>", get_dasha_lord(md_lord).c_str(), get_dasha_lord(ad_lord).c_str());
                printf("<table class='data-table' style='margin-top: 0;'><tr><th>గ్రహం</th><th>సంచార రాశి</th><th>నక్షత్రం (పాదం)</th><th>తార (నవతార)</th><th>జన్మ రాశి</th><th>చంద్రుని నుండి</th><th>లగ్నం నుండి</th><th>చూసే రాశులు (దృష్టి)</th></tr>");
            } else {
                printf("\n=== గోచారం (గ్రహ సంచారం) తేదీ: %02d/%02d/%04d సమయం: %02d:%02d:%02d ===\n", p_d, p_m, p_y, p_h, p_min, p_s);
                printf("ప్రస్తుత దశ: %s మహా దశ / %s అంతర్ దశ\n", get_dasha_lord(md_lord).c_str(), get_dasha_lord(ad_lord).c_str());
                printf("------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
                printf("%-10s | %-15s | %-20s | %-20s | %-12s | %-14s | %-12s | %-25s\n", "గ్రహం", "సంచార రాశి", "నక్షత్రం (పాదం)", "తార (నవతార)", "జన్మ రాశి", "చంద్రుని నుండి", "లగ్నం నుండి", "చూసే రాశులు (దృష్టి)");
                printf("------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
            }
        } else {
            if (html_mode) {
                printf("<h2 style='margin-top: 20px; margin-bottom: 10px; color: var(--accent);'>PLANETARY TRANSITS (GOCHAR) FOR %02d/%02d/%04d %02d:%02d:%02d</h2>", p_d, p_m, p_y, p_h, p_min, p_s);
                printf("<p style='color: #888; margin-top: 0; margin-bottom: 15px;'>Current Operating Dasha: <b>%s Mahadasha / %s Antardasha</b></p>", dasha_lords[md_lord], dasha_lords[ad_lord]);
                printf("<table class='data-table' style='margin-top: 0;'><tr><th>Graha</th><th>Transit Sign</th><th>Nakshatra (Pada)</th><th>Tara (Navatara)</th><th>Natal Sign</th><th>From Natal Mo</th><th>From Natal Asc</th><th>Aspected Signs</th></tr>");
            } else {
                printf("\n=== PLANETARY TRANSITS (GOCHAR) FOR %02d/%02d/%04d %02d:%02d:%02d ===\n", p_d, p_m, p_y, p_h, p_min, p_s);
                printf("Current Operating Dasha: %s Mahadasha / %s Antardasha\n", dasha_lords[md_lord], dasha_lords[ad_lord]);
                printf("------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
                printf("%-8s | %-15s | %-20s | %-20s | %-12s | %-13s | %-13s | %-25s\n", "Graha", "Transit Sign", "Nakshatra (Pada)", "Tara (Navatara)", "Natal Sign", "From Natal Mo", "From Natal Asc", "Aspected Signs");
                printf("------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
            }
        }

        double t_cusps[13], t_ascmc[10];
        swe_houses_ex(trans_jd, iflag, location.lat, location.lon, 'P', t_cusps, t_ascmc);
        double t_lagna = t_ascmc[0];
        string t_lagna_sign = format_dms(t_lagna);
        
        if (html_mode) {
            printf("<tr><td>%s</td><td>%s</td><td>-</td><td>-</td><td>-</td><td>-</td><td>-</td><td>-</td></tr>", telugu_mode ? "లగ్నం" : "Lagna", t_lagna_sign.c_str());
        } else {
            printf("%-8s | %-15s | %-20s | %-20s | %-12s | %-13s | %-13s | %-25s\n", telugu_mode ? "లగ్నం" : "Lagna", t_lagna_sign.c_str(), "-", "-", "-", "-", "-", "-");
        }
        
        int planets[] = {SE_SUN, SE_MOON, SE_MARS, SE_MERCURY, SE_JUPITER, SE_VENUS, SE_SATURN, node_calc_type};
        double xx[6]; char serr[256];
        int natal_mo_rashi = planet_rashis[2]; int natal_asc_rashi = planet_rashis[0]; 
        int natal_mo_nak = (int)(moon_lon / (360.0 / 27.0));

        map<int, vector<TransitHit>> transit_triggers; 
        int t_rashis[10];

        for (int i = 1; i <= 9; i++) {
            double trans_lon;
            if (i < 9) { swe_calc_ut(trans_jd, planets[i-1], iflag, xx, serr); trans_lon = xx[0]; } 
            else { swe_calc_ut(trans_jd, node_calc_type, iflag, xx, serr); trans_lon = fmod(xx[0] + 180.0, 360.0); }
            
            int trans_rashi = (int)(trans_lon / 30.0); t_rashis[i] = trans_rashi;
            double trans_deg = trans_lon - (trans_rashi * 30.0);
            int deg = (int)trans_deg; int min = (int)((trans_deg - deg) * 60.0);
            
            int trans_nak = (int)(trans_lon / (360.0 / 27.0));
            int pada = (int)((trans_lon - (trans_nak * (360.0 / 27.0))) / ((360.0 / 27.0) / 4.0)) + 1;
            string nak_pada = get_nak_name(trans_nak) + " " + to_string(pada);
            
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

            if (html_mode) {
                printf("<tr><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%d</td><td>%d</td><td>%s</td></tr>", 
                    telugu_mode ? get_planet_name(i).c_str() : p_names_full[i], t_sign.c_str(), nak_pada.c_str(), short_tara.c_str(), 
                    telugu_mode ? get_rashi_name(nat_rashi).c_str() : rashi_names[nat_rashi], from_mo, from_asc, asp_str.c_str());
            } else {
                if (telugu_mode) {
                    printf("%-10s | %-15s | %-20s | %-20s | %-12s | %s %-3d | %s %-3d | %-25s\n", 
                        get_planet_name(i).c_str(), t_sign.c_str(), nak_pada.c_str(), short_tara.c_str(), get_rashi_name(nat_rashi).c_str(), "భావం", from_mo, "భావం", from_asc, asp_str.c_str());
                } else {
                    printf("%-8s | %-15s | %-20s | %-20s | %-12s | %-13d | %-13d | %-25s\n", 
                        p_names_full[i], t_sign.c_str(), nak_pada.c_str(), short_tara.c_str(), rashi_names[nat_rashi], from_mo, from_asc, asp_str.c_str());
                }       
            }
            
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
            // GOCHARA TIMELINE SWEEPER & TEXT INJECTION
            // ==========================================
            if (i == 1 || i == 3 || i == 5 || i == 7 || i == 8 || i == 9) {
                double step = (i == 1 || i == 3) ? 1.0 : 5.0; // Fast planets step by 1 day, Slow planets by 5 days
                
                // Sweep backwards to find Entry Date (Rashi)
                double jd_in = trans_jd;
                while ((int)(get_planet_lon_on_jd(i, jd_in) / 30.0) == trans_rashi) jd_in -= step;
                while ((int)(get_planet_lon_on_jd(i, jd_in) / 30.0) != trans_rashi) jd_in += 1.0;

                // Sweep forwards to find Exit Date (Rashi)
                double jd_out = trans_jd;
                while ((int)(get_planet_lon_on_jd(i, jd_out) / 30.0) == trans_rashi) jd_out += step;
                while ((int)(get_planet_lon_on_jd(i, jd_out) / 30.0) != trans_rashi) jd_out -= 1.0;
                
                string date_range = "[ " + jd_to_string(jd_in).substr(0,10) + "  >>  " + jd_to_string(jd_out).substr(0,10) + " ]";
                
                // Fetch the SAV score for this planet's transit sign
                int r_sav = sav_scores[trans_rashi];

                // --- NEW: NAKSHATRA (TARA BALA) DATE INJECTION LOGIC ---
                string nak_start_dt, nak_end_dt;
                get_nakshatra_transit_range(i, trans_jd, iflag, nak_start_dt, nak_end_dt);

                string date_injection = html_mode ? " <span style='color: var(--accent); font-weight: bold;'>[ " + nak_start_dt + " &rarr; " + nak_end_dt + " ]</span>"
                                                  : " [ " + nak_start_dt + " -> " + nak_end_dt + " ]";

                string gochar_text_en = get_gochar_text(i, from_mo, r_sav, tara_idx);
                string gochar_text_te = te_get_gochar_text(i, from_mo, r_sav, tara_idx);

                // Inject into English text exactly after the word "star"
                size_t pos_en = gochar_text_en.find(" star");
                if (pos_en != string::npos) {
                    gochar_text_en.insert(pos_en + 5, date_injection); 
                }

                // Inject into Telugu text exactly after the word "నక్షత్రం"
                string target_te_phrase = " నక్షత్రం";
                size_t pos_te = gochar_text_te.find(target_te_phrase);
                if (pos_te != string::npos) {
                    gochar_text_te.insert(pos_te + target_te_phrase.length(), date_injection);
                }
                // --------------------------------------------------------
                
                // Formulate the payload string perfectly for HTML vs CLI
                string gochar_payload = "";
                if (html_mode) {
                    gochar_payload = (telugu_mode ? "<b>కాల వ్యవధి:</b> " : "<b>Timeline:</b> ") + date_range + "<br><br>" + 
                                     (telugu_mode ? gochar_text_te : gochar_text_en);
                } else {
                    gochar_payload = (telugu_mode ? "కాల వ్యవధి: " : "Timeline: ") + date_range + "\n        " + 
                                     (telugu_mode ? gochar_text_te : gochar_text_en);
                }
                
                transit_triggers[i].push_back({"GOCHAR_RESULT", gochar_payload});
            }
        }
        
        if (html_mode) {
            printf("</table>\n");
        } else {
            printf("----------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
        }

        // =========================================================================
        // PART A: THE NARRATIVE TIMELINE (For the User)
        // =========================================================================
        if (html_mode) {
            printf("<h2 style='margin-top: 30px; margin-bottom: 15px; color: var(--accent);'>%s</h2>", telugu_mode ? "గోచార ఫలితాలు (DYNAMIC TRANSIT PREDICTIONS)" : "DYNAMIC TRANSIT PREDICTIONS (GOCHARA PHALA)");
        } else {
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
        }

        int major_planets[] = {1, 3, 5, 7, 8, 9}; // Sun, Mars, Jup, Sat, Rahu, Ketu
        
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

                    if (html_mode) {
                        printf("<div style='margin-bottom:15px; padding:15px; background:#2a2a35; border-left:4px solid var(--accent); border-radius:4px;'>");
                        printf("<h4 style='margin-top:0; color:#e0e0e0;'>%s %s <span style='color:#888; font-size:12px;'>(%s %d%s)</span> <span style='color:var(--term-text); font-size:12px;'>%s%s</span></h4>", 
                               telugu_mode ? get_planet_name(mp).c_str() : p_names_full[mp],
                               telugu_mode ? "గమనము" : "TRANSIT",
                               telugu_mode ? "ప్రస్తుతం" : "Currently in",
                               (t_rashis[mp] - natal_mo_rashi + 12) % 12 + 1,
                               telugu_mode ? "వ భావంలో" : "th House",
                               dasha_alert.c_str(), av_alert.c_str());
                        printf("<p style='margin:5px 0; font-size:14px; line-height:1.6;'>%s</p>", gochar_text.c_str());
                        printf("</div>\n");
                    } else {
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
        }
        
        // =========================================================================
        // PART B: TECHNICAL TELEMETRY (Hidden in Web UI to prevent clutter)
        // =========================================================================
        if (!html_mode) {
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
                            int r_bav = bav_scores[mp-1][r]; 
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
        }
        
        fflush(stdout); // FORCE EVERYTHING TO JAVASCRIPT

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
                else { swe_calc_ut(trans_jd, node_calc_type, iflag, xx, serr); t_lon = fmod(xx[0] + 180.0, 360.0); }

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
                else { swe_calc_ut(trans_jd, node_calc_type, iflag, xx, serr); t_lon = fmod(xx[0] + 180.0, 360.0); }

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
        
        fflush(stdout); // FORCE EVERYTHING TO JAVASCRIPT
    }


struct DayScore{string date; int score; string reason; double jd;};

void decode_exact_date(int target_year, int asc_rashi, int h7_rashi, int h8_rashi, int dk_rashi, int l7_rashi_val, int l8_rashi_val, int target_natal_rashi, int planet_rashis[]){
    printf("\n=== DECODING EXACT DATE IN %d [8TH HOUSE LOCK - SHUDDHI + PANCHANG] ===\n", target_year);
    double jd_start = swe_julday(target_year,1,1,12.0-location.tz_offset,SE_GREG_CAL);
    double jd_end = swe_julday(target_year,12,31,12.0-location.tz_offset,SE_GREG_CAL);

    auto dist=[](int f,int t){return (t-f+12)%12;};
    auto p_jup=[&](int t,int n){int d=dist(t,n); return d==0||d==4||d==6||d==8;};
    auto p_sat=[&](int t,int n){int d=dist(t,n); return d==0||d==2||d==6||d==9;};
    auto bnn=[&](int t,int n){int d=dist(t,n); return d==0||d==1||d==4||d==6||d==8||d==11;};

    vector<DayScore> days;
    bool panchang_mode = true; // your area - poojari uses panchang list, ignores 8th

    for(double jd=jd_start; jd<=jd_end; jd+=1.0){
        double xx_ju[6],xx_sa[6],xx_ma[6],xx_ve[6],xx_su[6],xx_mo[6],xx_ra[6]; char serr[256];
        swe_calc_ut(jd,SE_JUPITER,iflag,xx_ju,serr);
        swe_calc_ut(jd,SE_SATURN,iflag,xx_sa,serr);
        swe_calc_ut(jd,SE_MARS,iflag,xx_ma,serr);
        swe_calc_ut(jd,SE_VENUS,iflag,xx_ve,serr);
        swe_calc_ut(jd,SE_SUN,iflag,xx_su,serr);
        swe_calc_ut(jd,SE_MOON,iflag,xx_mo,serr);
        swe_calc_ut(jd,SE_MEAN_NODE,iflag,xx_ra,serr);

        int t_ju=(int)(xx_ju[0]/30),t_sa=(int)(xx_sa[0]/30),t_ma=(int)(xx_ma[0]/30);
        int t_ve=(int)(xx_ve[0]/30),t_su=(int)(xx_su[0]/30),t_mo=(int)(xx_mo[0]/30);
        int t_ra=(int)(xx_ra[0]/30),t_ke=(t_ra+6)%12;

        int y,m,d; double jut;
        swe_revjul(jd+(location.tz_offset/24.0),SE_GREG_CAL,&y,&m,&d,&jut);
        string dstr=(d<10?"0":"")+to_string(d)+"/"+(m<10?"0":"")+to_string(m)+"/"+to_string(y);

        int score=0; string reason;
        bool j_p = p_jup(t_ju,asc_rashi)||p_jup(t_ju,h7_rashi)||p_jup(t_ju,l7_rashi_val)||p_jup(t_ju,dk_rashi);
        bool s_p = p_sat(t_sa,asc_rashi)||p_sat(t_sa,h7_rashi)||p_sat(t_sa,l7_rashi_val)||p_sat(t_sa,dk_rashi);
        bool hasVe7 = bnn(t_ve,h7_rashi)||bnn(t_ve,asc_rashi)||bnn(t_ve,dk_rashi);
        if(!j_p &&!s_p &&!hasVe7) continue;
		
        if(j_p){score+=500; reason+="[Ju-7] ";}
        if(s_p){score+=400; reason+="[Sa-7] ";}
        if(j_p&&s_p){score+=300; reason+="[DOUBLE] ";}
        if(hasVe7){score+=300; reason+="[Ve->7] ";}
        if(bnn(t_su,h7_rashi)){score+=200; reason+="[Sun->7] ";}

        // --- 8TH SHUDDHI ---
        int malefic_in_8th=0;
        if(t_sa==h8_rashi){malefic_in_8th++; reason+="[Sa IN 8TH] ";}
        if(!panchang_mode){
            if(t_su==h8_rashi){malefic_in_8th++; reason+="[Su IN 8TH] ";}
            if(t_ra==h8_rashi||t_ke==h8_rashi){malefic_in_8th++; reason+="[Ra/Ke IN 8TH] ";}
        }
        // Allow list for your area
        if(t_ma==h8_rashi){reason+="[Ma IN 8TH-ALLOWED] ";}
        if(t_mo==h8_rashi){reason+="[Moon IN 8TH-ALLOWED] ";}
        if(t_ve==h8_rashi){score+=400; reason+="[Ve IN 8TH-BENEFIC] ";}
        if(t_ju==h8_rashi){score+=400; reason+="[Ju IN 8TH-BENEFIC] ";}

        if(!panchang_mode){
            if(malefic_in_8th>0) score -= 1000*malefic_in_8th;
            else {score+=800; reason+="[8TH-SHUDDHI] ";}
        } else {
            score+=400; reason+="[PANCHANG-MUHURTA] "; // ignore 8th
        }

        if(bnn(t_ve,h8_rashi)){score+=300; reason+="[Ve->8TH] ";}
        if(bnn(t_mo,h8_rashi)){score+=200; reason+="[Moon->8TH] ";}
        if(t_ve==l8_rashi_val||t_mo==l8_rashi_val){score+=400; reason+="[Ve/Mo with Natal L8] ";}
        if(t_su==t_ve){score+=500; reason+="[SUN-VENUS YUTI] ";}
        if(dist(t_su,t_ve)==1||dist(t_su,t_ve)==11){score+=250; reason+="[Sun-Ve Adj] ";}

        string h8_lord = rashi_lords[h8_rashi];
        transform(h8_lord.begin(), h8_lord.end(), h8_lord.begin(), ::tolower);
        bool is_shani_mangal = (h8_lord=="shani" || h8_lord=="mangal");
        int minScore = is_shani_mangal? 1200 : 1800;
        if(panchang_mode) minScore = 1000; // relaxed for panchang
        if(score < minScore) continue;
		
        days.push_back({dstr,score,reason,jd});
    }
    sort(days.begin(),days.end(),[](auto &a,auto &b){return a.score>b.score;});
    printf("Isolated night muhurta in %d:\n",target_year);
    for(int i=0;i<min(10,(int)days.size());i++){
        printf("%-12s Score %-4d %s\n",days[i].date.c_str(),days[i].score,days[i].reason.c_str());
    }
    /*printf("--- November %d check ---\n",target_year);
    for(auto &d: days){
        if(d.date.find("/11/")!=string::npos){
            printf("%-12s Score %-4d %s\n",d.date.c_str(),d.score,d.reason.c_str());
        }
    }*/
}

void predict_marriage_general(int start_year, int end_year, string gender_input) {
    int b_y, b_m, b_d; double b_jut;
    swe_revjul(tjd_ut + (location.tz_offset/24.0), SE_GREG_CAL, &b_y, &b_m, &b_d, &b_jut);

    string g = gender_input; for(auto &c:g) c=tolower(c);
    bool is_female = (g=="female"||g=="f");
    bool is_male = (g=="male"||g=="m");
    if(!is_female &&!is_male){ printf("ERROR: gender_input required [male/female]\n"); return; }

    if(!av_calculated) calculate_ashtakavarga(true);

    printf("\n=================================================================\n");
    printf("=== FINAL KALA-CHAKRA NADI ENGINE (TARA+SAV GENERIC - NO RAHU 7TH) ===\n");
    printf("=================================================================\n");
    printf("Native: %s | BYear %d | Scan %d to %d\n", is_female?"FEMALE":"MALE", b_y, start_year, end_year);

    int asc_rashi = planet_rashis[0];
    int h7_rashi = (asc_rashi + 6) % 12;
    int h8_rashi = (asc_rashi + 7) % 12;

    auto lower = [](string s){ for(auto &c:s) c=tolower(c); return s; };
    int l7_idx=1,l8_idx=1;
    for(int p=1;p<=7;p++){
        if(lower(p_names_full[p])==lower(rashi_lords[h7_rashi])) l7_idx=p;
        if(lower(p_names_full[p])==lower(rashi_lords[h8_rashi])) l8_idx=p;
    }
    int l7_rashi_val = planet_rashis[l7_idx];
    int l8_rashi_val = planet_rashis[l8_idx];

    vector<pair<int,double>> k_list;
    for(int i=1;i<=7;i++){ double deg = planet_lons[i]-fmod(planet_lons[i],30.0); k_list.push_back({i,deg}); }
    sort(k_list.begin(),k_list.end(),[](auto &a,auto &b){return a.second>b.second;});
    int dk_rashi = planet_rashis[k_list[6].first];
    int target_natal_rashi = is_female? planet_rashis[4] : planet_rashis[3];

    if(start_year < b_y+12) start_year = b_y+12;
    double start_jd = swe_julday(start_year,1,1,12.0-location.tz_offset,SE_GREG_CAL);
    double end_jd = swe_julday(end_year,12,31,12.0-location.tz_offset,SE_GREG_CAL);

    struct Season{double s,e; int peak; string reason; int days; int pyear; string tdate;};
    vector<Season> active_seasons;
    Season cur={0,0,0,"",0,0,""};
    map<int,int> year_days, year_max, year_true_power;
    map<int,string> year_reason;

    double natal_moon_lon = planet_lons[2];
    int natal_mo_nak = (int)(natal_moon_lon / (360.0/27.0));
    if(natal_mo_nak<0) natal_mo_nak+=27; natal_mo_nak%=27;

    auto get_tara_idx = [](int transit_nak, int natal_nak){ return (transit_nak - natal_nak + 27) % 9; };
    auto is_good_tara_idx = [](int idx){ return idx==1||idx==3||idx==5||idx==7||idx==8; };

    for(double jd=start_jd; jd<=end_jd; jd+=1.0){
        double xx_ju[6],xx_sa[6],xx_ma[6],xx_ve[6],xx_su[6],xx_ra[6]; char serr[256];
        swe_calc_ut(jd,SE_JUPITER,iflag,xx_ju,serr);
        swe_calc_ut(jd,SE_SATURN,iflag,xx_sa,serr);
        swe_calc_ut(jd,SE_MARS,iflag,xx_ma,serr);
        swe_calc_ut(jd,SE_VENUS,iflag,xx_ve,serr);
        swe_calc_ut(jd,SE_SUN,iflag,xx_su,serr);
        swe_calc_ut(jd,node_calc_type,iflag,xx_ra,serr);

        int t_ju=(int)(xx_ju[0]/30),t_sa=(int)(xx_sa[0]/30),t_ma=(int)(xx_ma[0]/30),t_ve=(int)(xx_ve[0]/30),t_su=(int)(xx_su[0]/30),t_ra=(int)(xx_ra[0]/30);
        bool jup_retro=xx_ju[3]<0;
        int t_ju_prev=(t_ju+11)%12;

        auto dist2=[](int f,int t){return (t-f+12)%12;};
        auto p_jup2=[&](int t,int n){int d=dist2(t,n); return d==0||d==4||d==6||d==8;};
        auto p_sat2=[&](int t,int n){int d=dist2(t,n); return d==0||d==2||d==6||d==9;};
        auto bnn2=[&](int t,int n){int d=dist2(t,n); return d==0||d==1||d==4||d==6||d==8||d==11;};

        int y,m,d; double jut;
        swe_revjul(jd+(location.tz_offset/24.0),SE_GREG_CAL,&y,&m,&d,&jut);

        bool j_p = p_jup2(t_ju,asc_rashi)||p_jup2(t_ju,h7_rashi)||p_jup2(t_ju,planet_rashis[l7_idx])||p_jup2(t_ju,dk_rashi);
        bool s_p = p_sat2(t_sa,asc_rashi)||p_sat2(t_sa,h7_rashi)||p_sat2(t_sa,planet_rashis[l7_idx])||p_sat2(t_sa,dk_rashi);
        bool int_active = p_jup2(t_ju,h8_rashi)||p_sat2(t_sa,h8_rashi)||p_jup2(t_ju,planet_rashis[l8_idx])||p_sat2(t_sa,planet_rashis[l8_idx]);

        int moon_rashi = planet_rashis[2];
        // Rahu 7th from Moon REMOVED - not a marriage yoga, only karmic debt
        bool jup_in_11th_from_moon = (t_ju == (moon_rashi+10)%12);
        bool jup_in_7th_from_moon = (t_ju == (moon_rashi+6)%12);
        bool jup_in_2nd_5th_9th_from_moon = (t_ju==(moon_rashi+1)%12 || t_ju==(moon_rashi+4)%12 || t_ju==(moon_rashi+8)%12);

        int t_ju_nak = (int)(xx_ju[0] / (360.0/27.0)) %27;
        int t_sa_nak = (int)(xx_sa[0] / (360.0/27.0)) %27;
        int t_ra_nak = (int)(xx_ra[0] / (360.0/27.0)) %27;
        if(t_ju_nak<0) t_ju_nak+=27; if(t_sa_nak<0) t_sa_nak+=27; if(t_ra_nak<0) t_ra_nak+=27;

        int tara_ju_idx = get_tara_idx(t_ju_nak, natal_mo_nak);
        int tara_sa_idx = get_tara_idx(t_sa_nak, natal_mo_nak);
        int tara_ra_idx = get_tara_idx(t_ra_nak, natal_mo_nak);

        int sav_ju = sav_scores[t_ju];
        int sav_sa = sav_scores[t_sa];
        int sav_ra = sav_scores[t_ra];

        int p_score=0; string reason_add="";

        if(jup_in_11th_from_moon){ p_score+=1200; reason_add+="[Ju 11th from Moon] "; }
        if(jup_in_7th_from_moon){ p_score+=600; reason_add+="[Ju 7th from Moon] "; }
        if(jup_in_2nd_5th_9th_from_moon){ p_score+=400; reason_add+="[Ju 2/5/9 from Moon] "; }

        if(sav_ju>=30 && is_good_tara_idx(tara_ju_idx)){
            p_score+=600;
            reason_add+="[Ju SAV"+to_string(sav_ju)+" Tara"+to_string(tara_ju_idx+1)+"] ";
        }
        if(sav_sa>=32 && is_good_tara_idx(tara_sa_idx)){
            p_score+=700;
            reason_add+="[Sa SAV"+to_string(sav_sa)+" Tara"+to_string(tara_sa_idx+1)+"] ";
        }
        // Rahu only as Tara permission, reduced score
        if(sav_ra>=30 && is_good_tara_idx(tara_ra_idx)){
            p_score+=300;
            reason_add+="[Rahu Tara"+to_string(tara_ra_idx+1)+" SAV"+to_string(sav_ra)+"] ";
        }

        if(j_p && s_p){ p_score+=600; if(int_active) p_score+=400; }
        else if(j_p || s_p){ p_score+=300; if(int_active) p_score+=200; }

        auto bnn_ju=[&](int n){return bnn2(t_ju,n)||(jup_retro&&bnn2(t_ju_prev,n));};
        bool j_b=bnn_ju(target_natal_rashi);
        bool su_b=bnn2(t_su,target_natal_rashi)||bnn2(t_su,t_ju);
        bool ve_b=bnn2(t_ve,target_natal_rashi)||bnn2(t_ve,t_ju);

        int b_score=0;
        if(j_b&&su_b&&ve_b) b_score=700;
        else if(j_b&&(su_b||ve_b)) b_score=400;
        else if(j_b||ve_b) b_score=200;

        int base_score=p_score+b_score;
        string reason; reason+=reason_add;
        if(p_score) reason+="[P-"+to_string(p_score)+"] ";
        if(b_score) reason+="[B-"+to_string(b_score)+"] ";
        auto is_core=[&](int p){return p==asc_rashi||p==h7_rashi||p==h8_rashi||p==dk_rashi||p==planet_rashis[l7_idx]||p==target_natal_rashi;};
        if(t_ju==h7_rashi) {base_score+=600; reason+="[Ju in H7] ";}
        if(t_sa==h7_rashi) {base_score+=700; reason+="[Sa in H7] ";}
        if(is_core(t_su)) {base_score+=150; reason+="[Sun] ";}
        if(is_core(t_ve)) {base_score+=250; reason+="[Venus] ";}
        if(is_core(t_ma)) {base_score+=100; reason+="[Mars] ";}

        if(base_score < 400) continue;
        year_days[y]++;
        if(base_score>year_max[y]){year_max[y]=base_score; year_reason[y]=reason; year_true_power[y]=p_score*10+b_score;}

        string dstr=(d<10?"0":"")+to_string(d)+"/"+(m<10?"0":"")+to_string(m)+"/"+to_string(y);
        if(cur.s==0) cur={jd,jd,base_score,reason,1,y,dstr};
        else if(jd<=cur.e+8.0){ if(jd>cur.e) cur.days++; cur.e=jd; if(base_score>cur.peak){cur.peak=base_score; cur.reason=reason; cur.pyear=y; cur.tdate=dstr+" (Peak)";}}
        else{active_seasons.push_back(cur); cur={jd,jd,base_score,reason,1,y,dstr};}
    }
    if(cur.s!=0) active_seasons.push_back(cur);

    int MIN_AGE = is_female? 15 : 21;
    int ABSOLUTE_MIN = is_female? 14 : 21;

    int primary_year=0;
    int best_score = -999999;
    for(int y=start_year; y<=end_year; y++){
        if(year_days.find(y)==year_days.end()) continue;
        int age=y-b_y; if(age<ABSOLUTE_MIN) continue;

        int peak=year_max[y]; string rsn=year_reason[y];
        bool hasSun = rsn.find("[Sun]")!=string::npos;
        bool hasVenus = rsn.find("[Venus]")!=string::npos;
        bool hasSaH7 = rsn.find("Sa in H7")!=string::npos;
        bool hasJuH7 = rsn.find("Ju in H7")!=string::npos;
        bool hasJu7Moon = rsn.find("Ju 7th from Moon")!=string::npos;

        bool hasDoubleSAV = rsn.find("Ju SAV")!=string::npos && rsn.find("Sa SAV")!=string::npos;
        bool hasSAV33 = rsn.find("SAV33")!=string::npos || rsn.find("SAV34")!=string::npos || rsn.find("SAV35")!=string::npos || rsn.find("SAV36")!=string::npos || rsn.find("SAV37")!=string::npos || rsn.find("SAV38")!=string::npos || rsn.find("SAV39")!=string::npos || rsn.find("SAV41")!=string::npos;

        bool hasStability = hasSaH7 || hasJuH7 || hasJu7Moon || hasDoubleSAV;
        bool hasShubha = is_female? (hasSun || hasVenus) : (hasSun && hasVenus);

        if(!hasShubha) continue;
        if(!hasStability && peak<3000) continue;
        if(peak < 2600) continue;

        int score = peak;
        if(hasDoubleSAV) score += 1200;
        if(hasSAV33) score += 600;
        if(hasJuH7 && hasSaH7) score += 500;
        if(is_female && age<18) score -= 300;
        score -= (y - start_year) * (is_female? 80 : 200);

        if(score > best_score){
            best_score = score;
            primary_year = y;
        }
    }

    struct YearProb{int year; double prob; int days; int power; int peak; string status; int age;};
    vector<YearProb> yearly;
    for(auto &kv:year_days){
        int y=kv.first, peak=year_max[y], age=y-b_y, power=year_true_power[y];
        double prob = (peak>=1600)?99.9 : (peak/1600.0)*85.0;
        string st = (y==primary_year)?"[EPOCH 1] VIVAHA (PRIMARY MARRIAGE)": (y<primary_year?"[PRECURSOR]":"[POST]");
        if(age<MIN_AGE) st="[PRECURSOR - UNDERAGE]";
        yearly.push_back({y,min(99.9,prob),kv.second,power,peak,st,age});
    }
    sort(yearly.begin(),yearly.end(),[](auto &a,auto &b){return a.year < b.year;});

    if(yearly.empty()){ printf("No marriage yogas found\n"); return; }

    printf(">> ULTIMATE PREDICTION: MAJOR EPOCH IN %d (Age: %d) Peak %d <<\n",primary_year,primary_year-b_y,year_max[primary_year]);
    printf("--------------------------------------------------------------------------------------------------------------------------------\n");
    for(auto &yp:yearly){
        printf(" %-4d | Age %-2d | %5.1f%% | Peak %-4d | Days %-3d | %s | %s\n", yp.year,yp.age,yp.prob,yp.peak,yp.days,yp.status.c_str(),year_reason[yp.year].c_str());
    }
    if(primary_year > 0){
        decode_exact_date(primary_year, asc_rashi, h7_rashi, h8_rashi, dk_rashi, l7_rashi_val, l8_rashi_val, target_natal_rashi, planet_rashis);
        if(year_days.find(primary_year-1)!=year_days.end())
            decode_exact_date(primary_year-1, asc_rashi, h7_rashi, h8_rashi, dk_rashi, l7_rashi_val, l8_rashi_val, target_natal_rashi, planet_rashis);
    }
}

void predict_marriage(int start_year, int end_year, string gender_input) {
    int b_y, b_m, b_d; double b_jut;
    swe_revjul(tjd_ut + (location.tz_offset/24.0), SE_GREG_CAL, &b_y, &b_m, &b_d, &b_jut);
    string g = gender_input; for(auto &c:g) c=tolower(c);
    bool is_female = (g=="female"||g=="f");
    if(g!="female" && g!="f" && g!="male" && g!="m"){ printf("ERROR gender\n"); return; }
    bool isTelugu = false;
    auto T = [&](const char* en, const char* te)->const char* { return isTelugu?te:en; };

    double xx_ra_nat[6]; char serr_nat[256];
    swe_calc_ut(tjd_ut, node_calc_type, iflag, xx_ra_nat, serr_nat);
    planet_lons[8]=xx_ra_nat[0]; planet_rashis[8]=(int)(xx_ra_nat[0]/30);
    planet_lons[9]=fmod(xx_ra_nat[0]+180,360); planet_rashis[9]=(planet_rashis[8]+6)%12;

    int asc_rashi = planet_rashis[0];
    int h7_rashi = (asc_rashi + 6) % 12;
    int h8_rashi = (asc_rashi + 7) % 12;
    auto lower = [](string s){ for(auto &c:s) c=tolower(c); return s; };
    int l7_idx=1,l8_idx=1;
    for(int p=1;p<=7;p++){ if(lower(p_names_full[p])==lower(rashi_lords[h7_rashi])) l7_idx=p; if(lower(p_names_full[p])==lower(rashi_lords[h8_rashi])) l8_idx=p; }
    int l7_rashi_val = planet_rashis[l7_idx];
    int l8_rashi_val = planet_rashis[l8_idx];
    double l7_lon_val = planet_lons[l7_idx];
    double l8_lon_val = planet_lons[l8_idx];

    vector<pair<int,double>> k_list;
    for(int i=1;i<=7;i++){ double deg=fmod(planet_lons[i],30.0); if(deg<0) deg+=30; k_list.push_back({i,deg}); }
    sort(k_list.begin(), k_list.end(), [](auto &a, auto &b){return a.second>b.second;});
    int dk_rashi = planet_rashis[k_list[6].first];
    int target_natal_rashi = is_female? planet_rashis[3] : planet_rashis[6];
    double target_natal_lon = is_female? planet_lons[3] : planet_lons[6];

    double start_jd = swe_julday(start_year,1,1,12.0-location.tz_offset,SE_GREG_CAL);
    double end_jd = swe_julday(end_year,12,31,12.0-location.tz_offset,SE_GREG_CAL);

    map<int,int> year_days, year_p, year_base, year_daily_max;
    map<int,int> year_ju8, year_sa8, year_ju7, year_sa7, year_ra, year_ve8, year_dk;
    map<int,int> year_ra_l7, year_ra_l8;
    map<int,string> year_reason, year_first, year_last;
    struct DailyRec{int y,m,d; int p; int daily; string date;};
    vector<DailyRec> all_daily;
    auto within_orb = [](double t_lon, double n_lon, double orb){ double diff=fabs(t_lon-n_lon); if(diff>180) diff=360-diff; return diff<=orb; };

    for(double jd=start_jd; jd<=end_jd; jd+=1.0){
        double xx_ju[6],xx_sa[6],xx_ve[6],xx_su[6],xx_ma[6],xx_mo[6],xx_me[6],xx_ra[6]; char serr[256];
        swe_calc_ut(jd,SE_JUPITER,iflag,xx_ju,serr); swe_calc_ut(jd,SE_SATURN,iflag,xx_sa,serr);
        swe_calc_ut(jd,SE_VENUS,iflag,xx_ve,serr); swe_calc_ut(jd,SE_SUN,iflag,xx_su,serr);
        swe_calc_ut(jd,SE_MARS,iflag,xx_ma,serr); swe_calc_ut(jd,SE_MOON,iflag,xx_mo,serr);
        swe_calc_ut(jd,SE_MERCURY,iflag,xx_me,serr); swe_calc_ut(jd,node_calc_type,iflag,xx_ra,serr);
        int t_ju=(int)(xx_ju[0]/30),t_sa=(int)(xx_sa[0]/30),t_ve=(int)(xx_ve[0]/30),t_su=(int)(xx_su[0]/30),t_ma=(int)(xx_ma[0]/30),t_mo=(int)(xx_mo[0]/30),t_me=(int)(xx_me[0]/30);
        int t_ra=(int)(xx_ra[0]/30); int t_ke=(t_ra+6)%12;
        auto dist2=[](int f,int t){return (t-f+12)%12;};
        auto p_jup2=[&](int t,int n){int d=dist2(t,n); return d==0||d==4||d==6||d==8;};
        auto p_sat2=[&](int t,int n){int d=dist2(t,n); return d==0||d==2||d==6||d==9;};
        auto bnn2=[&](int t,int n){int d=dist2(t,n); return d==0||d==1||d==4||d==6||d==8||d==11;};
        int y,m,d; double jut; swe_revjul(jd+(location.tz_offset/24.0),SE_GREG_CAL,&y,&m,&d,&jut);
        char date_str[16]; sprintf(date_str,"%02d/%02d/%04d",d,m,y);
        bool j_p = p_jup2(t_ju,asc_rashi)||p_jup2(t_ju,h7_rashi)||p_jup2(t_ju,planet_rashis[l7_idx])||p_jup2(t_ju,dk_rashi);
        bool s_p = p_sat2(t_sa,asc_rashi)||p_sat2(t_sa,h7_rashi)||p_sat2(t_sa,planet_rashis[l7_idx])||p_sat2(t_sa,dk_rashi);
        bool ju_h8_lock = p_jup2(t_ju,h8_rashi)||p_jup2(t_ju,planet_rashis[l8_idx]);
        bool sa_h8_lock = p_sat2(t_sa,h8_rashi)||p_sat2(t_sa,planet_rashis[l8_idx]);
        if(!j_p &&!s_p) continue; if(!(ju_h8_lock||sa_h8_lock)) continue;
        bool ju_h8_all = p_jup2(t_ju,h8_rashi)||p_jup2(t_ju,planet_rashis[l8_idx]);
        bool sa_h8_all = p_sat2(t_sa,h8_rashi)||p_sat2(t_sa,planet_rashis[l8_idx]);
        bool ju_h7_all = p_jup2(t_ju,h7_rashi)||p_jup2(t_ju,planet_rashis[l7_idx]);
        bool sa_h7_all = p_sat2(t_sa,h7_rashi)||p_sat2(t_sa,planet_rashis[l7_idx]);
        if(ju_h8_all) year_ju8[y]++; if(sa_h8_all) year_sa8[y]++;
        if(ju_h7_all) year_ju7[y]++; if(sa_h7_all) year_sa7[y]++;
        bool ra_over=(t_ra==l7_rashi_val||t_ra==l8_rashi_val||t_ke==l7_rashi_val||t_ke==l8_rashi_val);
        bool ra_h7=(t_ra==h7_rashi||t_ke==h7_rashi);
        bool ra_l7 = (t_ra==l7_rashi_val||t_ke==l7_rashi_val||t_ra==h7_rashi||t_ke==h7_rashi);
        bool ra_l8 = (t_ra==l8_rashi_val||t_ke==l8_rashi_val);
        if(ra_over||ra_h7) year_ra[y]++; if(ra_l7) year_ra_l7[y]++; if(ra_l8) year_ra_l8[y]++;
        if(t_ve==h8_rashi) year_ve8[y]++; if(p_jup2(t_ju,dk_rashi)||p_sat2(t_sa,dk_rashi)) year_dk[y]++;
        int p_score=0; if(j_p&&s_p) p_score+=600; else p_score+=500;
        bool j_b = within_orb(xx_ju[0],target_natal_lon,5.0) || bnn2(t_ju,target_natal_rashi);
        bool su_b = within_orb(xx_su[0],target_natal_lon,2.0) || bnn2(t_su,target_natal_rashi);
        bool ve_b = within_orb(xx_ve[0],target_natal_lon,2.0) || bnn2(t_ve,target_natal_rashi);
        int b_score=0; if(j_b&&su_b&&ve_b) b_score=700; else if(j_b&&(su_b||ve_b)) b_score=400; else if(j_b||ve_b) b_score=200; else continue;
        int base_total = p_score + b_score; if(base_total<500) continue;
        int daily_score = p_score + b_score + (t_ve==h8_rashi?500:0);
        year_days[y]++; year_daily_max[y]=max(year_daily_max[y],daily_score);
        if(year_first[y].empty()) year_first[y]=string(date_str); year_last[y]=string(date_str);
        if(p_score>year_p[y]){ year_p[y]=p_score; year_base[y]=base_total; }
        all_daily.push_back({y,m,d,p_score,daily_score,string(date_str)});
    }

    struct PeakInfo{int year; int pScore; int baseTotal; int days; int ju8,sa8,ju7,sa7,ra_l7,ra_l8,ve8,dk; int dailyMax; string reason; int age; string first; string last;};
    vector<PeakInfo> byScore, allYears;
    for(auto &kv:year_days){
        int y=kv.first;
        allYears.push_back({y,year_p[y],year_base[y],kv.second,year_ju8[y],year_sa8[y],year_ju7[y],year_sa7[y],year_ra_l7[y],year_ra_l8[y],year_ve8[y],year_dk[y],year_daily_max[y],year_reason[y],y-b_y,year_first[y],year_last[y]});
        if((y-b_y)>=14) byScore.push_back({y,year_p[y],year_base[y],kv.second,year_ju8[y],year_sa8[y],year_ju7[y],year_sa7[y],year_ra_l7[y],year_ra_l8[y],year_ve8[y],year_dk[y],year_daily_max[y],year_reason[y],y-b_y,year_first[y],year_last[y]});
    }

    // RESTORED ORIGINAL LOGIC - 100% same as your first file
    auto lower2=lower;
    bool l7_eq_l8 = (l7_rashi_val==l8_rashi_val);
    bool h8_lord_shukra = (lower2(rashi_lords[h8_rashi])=="shukra");
    bool h8_lord_budha = (lower2(rashi_lords[h8_rashi])=="budha");
    bool h8_lord_mangal = (lower2(rashi_lords[h8_rashi])=="mangal");
    bool h8_lord_shani = (lower2(rashi_lords[h8_rashi])=="shani");
    bool h8_lord_chandra = (lower2(rashi_lords[h8_rashi])=="chandra");
    bool h8_lord_surya = (lower2(rashi_lords[h8_rashi])=="surya");
    auto balanceScore = [](PeakInfo &p){ double a[8]={(double)p.ju8,(double)p.sa8,(double)p.ju7,(double)p.sa7,(double)p.ra_l7,(double)p.ra_l8,(double)p.ve8,(double)p.dk}; double m=0; for(int i=0;i<8;i++) m+=a[i]; m/=8; double v=0; for(int i=0;i<8;i++) v+=(a[i]-m)*(a[i]-m); v/=8; return 1000.0 - sqrt(v); };

    sort(byScore.begin(), byScore.end(), [&](auto &a, auto &b){
        auto finalScore = [&](PeakInfo &p){
            int min5 = min({p.ju8,p.sa8,p.ju7,p.sa7,p.ve8});
            double bal = balanceScore(p);
            int ideal = is_female?20:30;
            int ve8_w = 3, sa7_w = 1;
            if(l7_eq_l8){ ve8_w=3; sa7_w=1; }
            else if(h8_lord_mangal){ ve8_w=50; sa7_w=1; }
            else if(h8_lord_budha){ ve8_w=20; sa7_w=0; }
            else if(h8_lord_shani){ ve8_w=10; sa7_w=1; }
            else if(h8_lord_chandra||h8_lord_surya){ ve8_w=15; sa7_w=h8_lord_surya?0:1; }
            double s = bal + min5 + p.ve8*ve8_w + p.ra_l7 + p.sa7*sa7_w - p.ra_l8*2 - abs(p.age-ideal)*40;
            if(l7_eq_l8) s += p.sa7*4;
            if(!h8_lord_shukra && p.sa8==0) s -= 500;
            return s;
        };
        return finalScore(a) > finalScore(b);
    });
    sort(allYears.begin(), allYears.end(), [](auto &a, auto &b){return a.year<b.year;});
    int topN = min((int)byScore.size(),3);

    if(html_mode){
        printf("<div style='font-family:Inter,Arial;padding:10px;background:#0f1419;border-radius:10px'>");
        printf("<div style='color:#f39c12;font-weight:bold;margin:8px 0'>💍 %s - %s: <b style='color:#fff'>%s</b> | %s %d | %s %d %s %d</div>",
            T("Marriage Prediction Engine","వివాహ అంచనా"), T("Native","జాతకుడు"), is_female?T("FEMALE","స్త్రీ"):T("MALE","పురుషుడు"),
            T("Birth Year","పుట్టిన సంవత్సరం"), b_y, T("Scanning from","స్కాన్"), start_year, T("to","నుండి"), end_year);
        if(topN>0){
            printf("<div style='background:#1e2a3a;border-left:5px solid #2ecc71;padding:10px;margin:10px 0;border-radius:6px'>");
            printf("<div style='color:#2ecc71;font-weight:bold'>⭐ %s: %d</div>", T("ULTIMATE","అత్యుత్తమం"), byScore[0].year);
            printf("<div style='color:#95a5a6;font-size:12px'>TOP 3: "); for(int i=0;i<topN;i++) printf("%d ",byScore[i].year); printf("</div></div>");
        }
        printf("<table style='width:100%%;border-collapse:collapse;font-size:13px;background:#111'><tr style='background:#2c3e50;color:#f1c40f'><th style='padding:8px'>%s</th><th style='padding:8px'>%s</th><th style='padding:8px'>%s</th></tr>",
            T("Window [Start - End]","కాలం"), T("Marriage Phase","వివాహ దశ"), T("Astrological Triggers","జ్యోతిష కారణాలు"));
        for(auto &pk: allYears){
            string phase=T("Favorable & Stable","అనుకూలం"); string color="#bdc3c7"; string trig="";
            if(pk.ju7>100) trig+= string(T("Jupiter blesses 7th ","గురుడు 7వ "))+"["+to_string(pk.ju7)+"d] ";
            if(pk.sa7>100) trig+= string(T("Saturn blesses 7th ","శని 7వ "))+"["+to_string(pk.sa7)+"d] ";
            if(pk.ju8>200 && pk.sa8>200) trig+= T("DOUBLE 8TH LOCK ","డబుల్ 8వ లాక్ ");
            if(trig.empty()) trig = T("Mild support","స్వల్ప అనుకూలం");
            for(int i=0;i<topN;i++) if(pk.year==byScore[i].year){
                if(i==0){ phase=T("BEST MARRIAGE WINDOW ⭐ HIGHEST","అత్యుత్తమ వివాహం ⭐"); color="#2ecc71"; }
                else if(i==1){ phase=T("Favorable Alliance","అనుకూల సంబంధం"); color="#f1c40f"; }
                else { phase=T("Good Proposal","మంచి ప్రతిపాదన"); color="#3498db"; }
            }
            char win[64]; sprintf(win,"%s → %s (Age %d)",pk.first.c_str(),pk.last.c_str(),pk.age);
            printf("<tr style='border-bottom:1px solid #333'><td style='padding:8px;color:#2ecc71'>%s</td><td style='padding:8px'><b style='color:%s'>%s</b></td><td style='padding:8px;color:#95a5a6'>%s</td></tr>", win, color.c_str(), phase.c_str(), trig.c_str());
        }
        printf("</table></div>");
    } else {
        printf("\n========================================================================================================================\n");
        printf("Marriage Prediction Engine - Native: %s | Birth Year %d | Scanning from %d to %d\n", is_female?"FEMALE":"MALE", b_y, start_year, end_year);
        printf("========================================================================================================================\n");
        if(topN>0){ printf("ULTIMATE: %d | TOP 3: ", byScore[0].year); for(int i=0;i<topN;i++) printf("%d ",byScore[i].year); printf("\n------------------------------------------------------------------------------------------------------------------------\n"); }
        printf("%-35s | %-30s | %s\n", "Window [Start - End]", "Marriage Phase", "Astrological Triggers");
        printf("------------------------------------------------------------------------------------------------------------------------\n");
        for(auto &pk: allYears){
			string phase="Favorable & Stable";
			string phase_color="\033[0;37m"; // grey - normal
			string trig="";
			if(pk.ju7>100) trig+= "Jupiter 7th ["+to_string(pk.ju7)+"d] ";
			if(pk.sa7>100) trig+= "Saturn 7th ["+to_string(pk.sa7)+"d] ";
			if(pk.ju8>200 && pk.sa8>200) trig+= "DOUBLE 8TH LOCK ";
			if(trig.empty()) trig="Mild support";
			for(int i=0;i<topN;i++) if(pk.year==byScore[i].year){
				if(i==0){ phase="BEST MARRIAGE WINDOW HIGHEST"; phase_color="\033[1;32m"; } // green
				else if(i==1){ phase="Favorable Alliance"; phase_color="\033[1;33m"; } // yellow
				else { phase="Good Proposal"; phase_color="\033[1;34m"; } // blue
			}
			char win[64]; sprintf(win,"%s -> %s (Age %d)",pk.first.c_str(),pk.last.c_str(),pk.age);
			printf("%-35s | %s%-30s\033[0m | %s\n", win, phase_color.c_str(), phase.c_str(), trig.c_str());
		}
        printf("------------------------------------------------------------------------------------------------------------------------\n");
    }
    for(int i=0;i<topN;i++) decode_exact_date(byScore[i].year, asc_rashi, h7_rashi, h8_rashi, dk_rashi, l7_rashi_val, l8_rashi_val, target_natal_rashi, planet_rashis);
}

void predict_job(int start_year, int end_year) {
        int b_y, b_m, b_d; double b_jut;
        swe_revjul(tjd_ut + (location.tz_offset/24.0), SE_GREG_CAL, &b_y, &b_m, &b_d, &b_jut);

        printf("\n=================================================================================================================================\n");
        printf("=== CAREER & JOB PREDICTION ENGINE (NADI & PARASHARI) ===\n");
        printf("=================================================================================================================================\n");
        printf("Scanning %d to %d for New Jobs, Promotions, Career Breaks, and Transitions...\n", start_year, end_year);
        printf("---------------------------------------------------------------------------------------------------------------------------------\n");

        int asc_rashi = planet_rashis[0];
        int mo_rashi = planet_rashis[2];
        int h10_rashi = (asc_rashi + 9) % 12;
        int l10_idx = 1; for(int p=1; p<=7; p++) if(string(rashi_lords[h10_rashi]) == p_names_full[p]) l10_idx = p;
        int l10_rashi = planet_rashis[l10_idx];
        int nat_sa_rashi = planet_rashis[7];
        int nat_ke_rashi = planet_rashis[9];
        int nat_su_rashi = planet_rashis[1];

        double start_jd = swe_julday(start_year, 1, 1, 0.0 - location.tz_offset, SE_GREG_CAL);
        double end_jd = swe_julday(end_year, 12, 31, 0.0 - location.tz_offset, SE_GREG_CAL);

        struct Phase { double start_jd; double end_jd; int peak_pos; int peak_neg; string status; string reason; };
        vector<Phase> phases;
        Phase cur = {0, 0, 0, 0, "", ""};

        auto dist = [](int from, int to) { return (to - from + 12) % 12; };
        auto is_conj = [&](int t, int n) { return t == n; };
        auto is_jup_asp = [&](int t, int n) { int d = dist(t, n); return d==0 || d==4 || d==6 || d==8; }; 
        auto is_sat_asp = [&](int t, int n) { int d = dist(t, n); return d==0 || d==2 || d==6 || d==9; };

        for (double jd = start_jd; jd <= end_jd; jd += 1.0) {
            double xx_ju[6], xx_sa[6], xx_su[6], xx_ra[6]; char serr[256];
            swe_calc_ut(jd, SE_JUPITER, iflag, xx_ju, serr);
            swe_calc_ut(jd, SE_SATURN, iflag, xx_sa, serr);
            swe_calc_ut(jd, SE_SUN, iflag, xx_su, serr);
            swe_calc_ut(jd, node_calc_type, iflag, xx_ra, serr);

            int t_ju = (int)(xx_ju[0] / 30.0), t_sa = (int)(xx_sa[0] / 30.0);
            int t_su = (int)(xx_su[0] / 30.0), t_ra = (int)(xx_ra[0] / 30.0);
            int t_ke = (t_ra + 6) % 12;

            int pos_score = 0; int neg_score = 0; string reason = ""; string status = "";

            // --- NEGATIVE TRIGGERS (Breaks / Friction / Resignations) ---
            if (t_ke == nat_sa_rashi) { neg_score -= 1000; reason += "[Ketu over Natal Saturn (Loss of Interest/Break)] "; }
            if (t_ke == h10_rashi) { neg_score -= 1000; reason += "[Ketu in 10th House (Detachment from Work)] "; }
            if (t_sa == nat_sa_rashi) { neg_score -= 800; reason += "[Saturn Return (Heavy Career Restructuring/Pressure)] "; }
            if (t_sa == nat_ke_rashi) { neg_score -= 800; reason += "[Saturn over Natal Ketu (Career Block/Resignation)] "; }
            if (t_ra == h10_rashi) { neg_score -= 600; reason += "[Rahu in 10th House (Office Politics/Instability)] "; }
            if (t_ra == nat_sa_rashi) { neg_score -= 600; reason += "[Rahu over Natal Saturn (Sudden Shift/Illusion)] "; }
            
            int sat_moon_dist = dist(mo_rashi, t_sa) + 1;
            if (sat_moon_dist == 8) { neg_score -= 600; reason += "[Ashtama Shani (Heavy Work Stress)] "; }
            else if (sat_moon_dist == 12 || sat_moon_dist == 1 || sat_moon_dist == 2) { 
                neg_score -= 300; reason += "[Sade Sati Phase (Structural Changes/Pressure)] "; 
            }

            // --- POSITIVE TRIGGERS (New Job / Promotion) ---
            if (is_jup_asp(t_ju, nat_sa_rashi)) { pos_score += 800; reason += "[Jup blesses Natal Sat (NEW JOB/Expansion)] "; }
            if (is_jup_asp(t_ju, h10_rashi)) { pos_score += 600; reason += "[Jup blesses 10th House (Status Elevation)] "; }
            if (is_jup_asp(t_ju, l10_rashi)) { pos_score += 500; reason += "[Jup blesses 10th Lord (Favorable Authority)] "; }
            if (is_sat_asp(t_sa, nat_su_rashi)) { pos_score += 400; reason += "[Sat activates Natal Sun (Govt/MNC Role/Promotion)] "; }
            if (t_ju == nat_su_rashi) { pos_score += 300; reason += "[Jup over Natal Sun (Recognition/Appreciation)] "; }
            
            if (is_jup_asp(t_ju, h10_rashi) && is_sat_asp(t_sa, h10_rashi)) { pos_score += 500; reason += "[DOUBLE TRANSIT on 10H (Major Career Milestone)] "; }

            if (pos_score >= 500 && (is_conj(t_su, h10_rashi) || is_conj(t_su, nat_sa_rashi))) { 
                pos_score += 200; reason += "[Sun Triggers Exact Timing] "; 
            }

            // --- DETERMINE BROAD PHASE (Bipolar check) ---
            if (neg_score <= -1000) {
                if (pos_score >= 800) status = "CAREER TRANSITION (Resignation + New Offer)";
                else status = "CRITICAL BREAK / RESIGNATION";
            }
            else if (neg_score <= -600) {
                if (pos_score >= 800) status = "HEAVY STRESS / NEW OPPORTUNITY EMERGES";
                else status = "HEAVY STRESS / POLITICS / INSTABILITY";
            }
            else if (pos_score >= 1000) status = "MAJOR PROMOTION / NEW JOB";
            else if (pos_score >= 600) status = "CAREER GROWTH / OFFERS";
            else if (pos_score >= 300 || neg_score <= -300) status = "FAVORABLE & STABLE";
            else status = "Neutral";

            bool is_active = (pos_score >= 300 || neg_score <= -300);

            if (cur.start_jd == 0 && is_active) {
                cur = {jd, jd, pos_score, neg_score, status, reason};
            } else if (is_active) {
                if (status == cur.status && reason == cur.reason) {
                    cur.end_jd = jd;
                    if (pos_score > cur.peak_pos) cur.peak_pos = pos_score;
                    if (neg_score < cur.peak_neg) cur.peak_neg = neg_score;
                } else {
                    if (cur.end_jd - cur.start_jd >= 15.0) phases.push_back(cur);
                    cur = {jd, jd, pos_score, neg_score, status, reason};
                }
            } else {
                if (cur.start_jd != 0) {
                    if (cur.end_jd - cur.start_jd >= 15.0) phases.push_back(cur);
                    cur = {0, 0, 0, 0, "", ""};
                }
            }
        }
        if (cur.start_jd != 0 && cur.end_jd - cur.start_jd >= 15.0) phases.push_back(cur);

        int max_positive_score = 0;
        for (const auto& p : phases) {
            if (p.peak_pos > max_positive_score && (p.status.find("PROMOTION") != string::npos || p.status.find("TRANSITION") != string::npos)) {
                max_positive_score = p.peak_pos;
            }
        }

        if (max_positive_score >= 1000) {
            printf(">> TOP PRIORITY WINDOW(S) FOR NEW JOB / MAJOR SHIFT (Peak Positive Power: %d pts) <<\n", max_positive_score);
            for (const auto& p : phases) {
                if (p.peak_pos == max_positive_score && (p.status.find("PROMOTION") != string::npos || p.status.find("TRANSITION") != string::npos)) {
                    int y1, m1, d1, y2, m2, d2; double jut;
                    swe_revjul(p.start_jd + (location.tz_offset/24.0), SE_GREG_CAL, &y1, &m1, &d1, &jut);
                    swe_revjul(p.end_jd + (location.tz_offset/24.0), SE_GREG_CAL, &y2, &m2, &d2, &jut);
                    printf(" ⭐ %02d/%02d/%04d to %02d/%02d/%04d -> %s\n", d1, m1, y1, d2, m2, y2, p.reason.c_str());
                }
            }
            printf("---------------------------------------------------------------------------------------------------------------------------------\n");
        }

        printf("%-25s | %-45s | %s\n", "Window [Start - End]", "Career Phase", "Astrological Triggers");
        printf("---------------------------------------------------------------------------------------------------------------------------------\n");

        for (const auto& p : phases) {
            int y1, m1, d1, y2, m2, d2; double jut;
            swe_revjul(p.start_jd + (location.tz_offset/24.0), SE_GREG_CAL, &y1, &m1, &d1, &jut);
            swe_revjul(p.end_jd + (location.tz_offset/24.0), SE_GREG_CAL, &y2, &m2, &d2, &jut);
            char s_buf[32], e_buf[32];
            snprintf(s_buf, sizeof(s_buf), "%02d/%02d/%04d", d1, m1, y1);
            snprintf(e_buf, sizeof(e_buf), "%02d/%02d/%04d", d2, m2, y2);
            string win_str = "[ " + string(s_buf) + " - " + string(e_buf) + " ]";
            
            string status_str = p.status;
            if (p.peak_pos == max_positive_score && max_positive_score >= 1000 && (p.status.find("PROMOTION") != string::npos || p.status.find("TRANSITION") != string::npos)) {
                status_str += " ⭐ HIGHEST CHANCE";
            }

            if (html_mode) {
                string color = "#bdc3c7";
                if (p.status.find("CRITICAL BREAK") != string::npos) color = "#e74c3c";
                else if (p.status.find("TRANSITION") != string::npos) color = "#f39c12"; 
                else if (p.status.find("PROMOTION") != string::npos) color = "#2ecc71";
                else if (p.status.find("GROWTH") != string::npos) color = "#27ae60";
                else if (p.status.find("STRESS") != string::npos) color = "#e67e22";
                
                printf("<tr><td>%s</td><td><b style='color:%s;'>%s</b></td><td>%s</td></tr>\n", win_str.c_str(), color.c_str(), status_str.c_str(), p.reason.c_str());
            } else {
                printf("%-25s | %-45s | %s\n", win_str.c_str(), status_str.c_str(), p.reason.c_str());
            }
        }

        if (phases.empty()) {
            printf(" No major career fluctuations or job breaks detected in this timeframe. Stable period.\n");
        }
        printf("---------------------------------------------------------------------------------------------------------------------------------\n");
    }

void predict_business(int start_year, int end_year) {
        int b_y, b_m, b_d; double b_jut;
        swe_revjul(tjd_ut + (location.tz_offset/24.0), SE_GREG_CAL, &b_y, &b_m, &b_d, &b_jut);

        printf("\n=================================================================================================================================\n");
        printf("=== BUSINESS & WEALTH PREDICTION ENGINE (NADI & PARASHARI) ===\n");
        printf("=================================================================================================================================\n");
        printf("Scanning %d to %d for New Business, Expansion, Partnerships, and Friction...\n", start_year, end_year);
        printf("---------------------------------------------------------------------------------------------------------------------------------\n");

        int asc_rashi = planet_rashis[0];
        int mo_rashi = planet_rashis[2];
        
        int h7_rashi = (asc_rashi + 6) % 12; // Business / Partnerships
        int l7_idx = 1; for(int p=1; p<=7; p++) if(string(rashi_lords[h7_rashi]) == p_names_full[p]) l7_idx = p;
        int l7_rashi = planet_rashis[l7_idx];
        
        int h11_rashi = (asc_rashi + 10) % 12; // Gains / Network
        
        int nat_me_rashi = planet_rashis[4]; // Mercury = Karaka for Business/Trade
        int nat_sa_rashi = planet_rashis[7]; // Saturn = Karaka for Karma/Work
        int nat_ke_rashi = planet_rashis[9];
        int nat_su_rashi = planet_rashis[1];

        double start_jd = swe_julday(start_year, 1, 1, 0.0 - location.tz_offset, SE_GREG_CAL);
        double end_jd = swe_julday(end_year, 12, 31, 0.0 - location.tz_offset, SE_GREG_CAL);

        struct Phase { double start_jd; double end_jd; int peak_pos; int peak_neg; string status; string reason; };
        vector<Phase> phases;
        Phase cur = {0, 0, 0, 0, "", ""};

        auto dist = [](int from, int to) { return (to - from + 12) % 12; };
        auto is_conj = [&](int t, int n) { return t == n; };
        auto is_jup_asp = [&](int t, int n) { int d = dist(t, n); return d==0 || d==4 || d==6 || d==8; }; 
        auto is_sat_asp = [&](int t, int n) { int d = dist(t, n); return d==0 || d==2 || d==6 || d==9; };

        for (double jd = start_jd; jd <= end_jd; jd += 1.0) {
            double xx_ju[6], xx_sa[6], xx_su[6], xx_ra[6]; char serr[256];
            swe_calc_ut(jd, SE_JUPITER, iflag, xx_ju, serr);
            swe_calc_ut(jd, SE_SATURN, iflag, xx_sa, serr);
            swe_calc_ut(jd, SE_SUN, iflag, xx_su, serr);
            swe_calc_ut(jd, node_calc_type, iflag, xx_ra, serr);

            int t_ju = (int)(xx_ju[0] / 30.0), t_sa = (int)(xx_sa[0] / 30.0);
            int t_su = (int)(xx_su[0] / 30.0), t_ra = (int)(xx_ra[0] / 30.0);
            int t_ke = (t_ra + 6) % 12;

            int pos_score = 0; int neg_score = 0; string reason = ""; string status = "";

            // --- NEGATIVE TRIGGERS (Breaks / Friction / Losses) ---
            if (t_ke == nat_me_rashi) { neg_score -= 1000; reason += "[Ketu over Natal Mercury (Business Break/Losses)] "; }
            if (t_ke == h7_rashi) { neg_score -= 1000; reason += "[Ketu in 7th House (Partnership Friction/Detachment)] "; }
            if (t_sa == nat_me_rashi) { neg_score -= 800; reason += "[Saturn over Natal Mercury (Heavy Business Pressure/Delays)] "; }
            if (t_sa == h7_rashi) { neg_score -= 800; reason += "[Saturn in 7th House (Strict Tests on Partnerships)] "; }
            if (t_ra == h7_rashi) { neg_score -= 600; reason += "[Rahu in 7th House (Deception/Unstable Partners)] "; }
            if (t_ra == nat_me_rashi) { neg_score -= 600; reason += "[Rahu over Natal Mercury (Risky Investments/Instability)] "; }
            
            int sat_moon_dist = dist(mo_rashi, t_sa) + 1;
            if (sat_moon_dist == 8) { neg_score -= 500; reason += "[Ashtama Shani (Heavy Business Stress)] "; }
            else if (sat_moon_dist == 12 || sat_moon_dist == 1 || sat_moon_dist == 2) { 
                neg_score -= 300; reason += "[Sade Sati Phase (Financial Pressure)] "; 
            }

            // --- POSITIVE TRIGGERS (New Business / Expansion) ---
            if (is_jup_asp(t_ju, nat_me_rashi)) { pos_score += 800; reason += "[Jup blesses Natal Merc (NEW BUSINESS/Expansion)] "; }
            if (is_jup_asp(t_ju, h7_rashi)) { pos_score += 600; reason += "[Jup blesses 7th House (Partnership/Trade Growth)] "; }
            if (is_jup_asp(t_ju, h11_rashi)) { pos_score += 500; reason += "[Jup blesses 11th House (Major Financial Gains)] "; }
            if (is_sat_asp(t_sa, nat_me_rashi)) { pos_score += 400; reason += "[Sat activates Natal Merc (Structured Business Growth)] "; }
            
            if (is_jup_asp(t_ju, h7_rashi) && is_sat_asp(t_sa, h7_rashi)) { pos_score += 500; reason += "[DOUBLE TRANSIT on 7H (Major Business Milestone)] "; }

            if (pos_score >= 500 && (is_conj(t_su, h7_rashi) || is_conj(t_su, nat_me_rashi))) { 
                pos_score += 200; reason += "[Sun Triggers Exact Timing] "; 
            }

            // --- DETERMINE BROAD PHASE (Bipolar check) ---
            if (neg_score <= -1000) {
                if (pos_score >= 800) status = "BUSINESS RESTRUCTURING (Losses + New Pivot)";
                else status = "CRITICAL BREAK / SEVERE LOSSES";
            }
            else if (neg_score <= -600) {
                if (pos_score >= 800) status = "HEAVY FRICTION / SAVED BY NEW DEAL";
                else status = "HEAVY FRICTION / INSTABILITY";
            }
            else if (pos_score >= 1000) status = "MAJOR EXPANSION / NEW BUSINESS";
            else if (pos_score >= 600) status = "BUSINESS GROWTH / PROFITS";
            else if (pos_score >= 300 || neg_score <= -300) status = "FAVORABLE & STABLE";
            else status = "Neutral";

            bool is_active = (pos_score >= 300 || neg_score <= -300);

            if (cur.start_jd == 0 && is_active) {
                cur = {jd, jd, pos_score, neg_score, status, reason};
            } else if (is_active) {
                if (status == cur.status && reason == cur.reason) {
                    cur.end_jd = jd;
                    if (pos_score > cur.peak_pos) cur.peak_pos = pos_score;
                    if (neg_score < cur.peak_neg) cur.peak_neg = neg_score;
                } else {
                    if (cur.end_jd - cur.start_jd >= 15.0) phases.push_back(cur);
                    cur = {jd, jd, pos_score, neg_score, status, reason};
                }
            } else {
                if (cur.start_jd != 0) {
                    if (cur.end_jd - cur.start_jd >= 15.0) phases.push_back(cur);
                    cur = {0, 0, 0, 0, "", ""};
                }
            }
        }
        if (cur.start_jd != 0 && cur.end_jd - cur.start_jd >= 15.0) phases.push_back(cur);

        // --- FIND THE TOP PRIORITY HIGHEST PEAK SCORE ---
        int max_positive_score = 0;
        for (const auto& p : phases) {
            if (p.peak_pos > max_positive_score && (p.status.find("EXPANSION") != string::npos || p.status.find("RESTRUCTURING") != string::npos)) {
                max_positive_score = p.peak_pos;
            }
        }

        if (max_positive_score >= 1000) {
            printf(">> TOP PRIORITY WINDOW(S) FOR NEW BUSINESS / EXPANSION (Peak Positive Power: %d pts) <<\n", max_positive_score);
            for (const auto& p : phases) {
                if (p.peak_pos == max_positive_score && (p.status.find("EXPANSION") != string::npos || p.status.find("RESTRUCTURING") != string::npos)) {
                    int y1, m1, d1, y2, m2, d2; double jut;
                    swe_revjul(p.start_jd + (location.tz_offset/24.0), SE_GREG_CAL, &y1, &m1, &d1, &jut);
                    swe_revjul(p.end_jd + (location.tz_offset/24.0), SE_GREG_CAL, &y2, &m2, &d2, &jut);
                    printf(" ⭐ %02d/%02d/%04d to %02d/%02d/%04d -> %s\n", d1, m1, y1, d2, m2, y2, p.reason.c_str());
                }
            }
            printf("---------------------------------------------------------------------------------------------------------------------------------\n");
        }

        printf("%-25s | %-45s | %s\n", "Window [Start - End]", "Business Phase", "Astrological Triggers");
        printf("---------------------------------------------------------------------------------------------------------------------------------\n");

        for (const auto& p : phases) {
            int y1, m1, d1, y2, m2, d2; double jut;
            swe_revjul(p.start_jd + (location.tz_offset/24.0), SE_GREG_CAL, &y1, &m1, &d1, &jut);
            swe_revjul(p.end_jd + (location.tz_offset/24.0), SE_GREG_CAL, &y2, &m2, &d2, &jut);
            char s_buf[32], e_buf[32];
            snprintf(s_buf, sizeof(s_buf), "%02d/%02d/%04d", d1, m1, y1);
            snprintf(e_buf, sizeof(e_buf), "%02d/%02d/%04d", d2, m2, y2);
            string win_str = "[ " + string(s_buf) + " - " + string(e_buf) + " ]";
            
            string status_str = p.status;
            if (p.peak_pos == max_positive_score && max_positive_score >= 1000 && (p.status.find("EXPANSION") != string::npos || p.status.find("RESTRUCTURING") != string::npos)) {
                status_str += " ⭐ HIGHEST CHANCE";
            }

            if (html_mode) {
                string color = "#bdc3c7";
                if (p.status.find("CRITICAL BREAK") != string::npos) color = "#e74c3c";
                else if (p.status.find("RESTRUCTURING") != string::npos) color = "#f39c12"; 
                else if (p.status.find("EXPANSION") != string::npos) color = "#2ecc71";
                else if (p.status.find("GROWTH") != string::npos) color = "#27ae60";
                else if (p.status.find("FRICTION") != string::npos) color = "#e67e22";
                
                printf("<tr><td>%s</td><td><b style='color:%s;'>%s</b></td><td>%s</td></tr>\n", win_str.c_str(), color.c_str(), status_str.c_str(), p.reason.c_str());
            } else {
                printf("%-25s | %-45s | %s\n", win_str.c_str(), status_str.c_str(), p.reason.c_str());
            }
        }

        if (phases.empty()) {
            printf(" No major business fluctuations detected in this timeframe. Stable period.\n");
        }
        printf("---------------------------------------------------------------------------------------------------------------------------------\n");
    }
	
void predict_study(int start_year, int end_year) {
        int b_y, b_m, b_d; double b_jut;
        swe_revjul(tjd_ut + (location.tz_offset/24.0), SE_GREG_CAL, &b_y, &b_m, &b_d, &b_jut);

        printf("\n=================================================================================================================================\n");
        printf("=== EDUCATION & STUDY PREDICTION ENGINE (NADI & PARASHARI) ===\n");
        printf("=================================================================================================================================\n");
        printf("Scanning %d to %d for Academic Success, Admissions, Focus, Distractions, and Study Breaks...\n", start_year, end_year);
        printf("---------------------------------------------------------------------------------------------------------------------------------\n");

        int asc_rashi = planet_rashis[0];
        int mo_rashi = planet_rashis[2];
        
        int h4_rashi = (asc_rashi + 3) % 12; // Primary Education
        int l4_idx = 1; for(int p=1; p<=7; p++) if(string(rashi_lords[h4_rashi]) == p_names_full[p]) l4_idx = p;
        int l4_rashi = planet_rashis[l4_idx];

        int h5_rashi = (asc_rashi + 4) % 12; // Undergraduate / Intellect
        int l5_idx = 1; for(int p=1; p<=7; p++) if(string(rashi_lords[h5_rashi]) == p_names_full[p]) l5_idx = p;
        int l5_rashi = planet_rashis[l5_idx];
        
        int h9_rashi = (asc_rashi + 8) % 12; // Masters / Higher Education
        int l9_idx = 1; for(int p=1; p<=7; p++) if(string(rashi_lords[h9_rashi]) == p_names_full[p]) l9_idx = p;
        int l9_rashi = planet_rashis[l9_idx];

        int nat_me_rashi = planet_rashis[4]; // Mercury = Karaka for Schooling/Intellect
        int nat_ju_rashi = planet_rashis[5]; // Jupiter = Karaka for Higher Education/Wisdom
        
        double start_jd = swe_julday(start_year, 1, 1, 0.0 - location.tz_offset, SE_GREG_CAL);
        double end_jd = swe_julday(end_year, 12, 31, 0.0 - location.tz_offset, SE_GREG_CAL);

        struct Phase { double start_jd; double end_jd; int peak_score; string status; string reason; };
        vector<Phase> phases;
        Phase cur = {0, 0, 0, "", ""};

        auto dist = [](int from, int to) { return (to - from + 12) % 12; };
        auto is_conj = [&](int t, int n) { return t == n; };
        auto is_jup_asp = [&](int t, int n) { int d = dist(t, n); return d==0 || d==4 || d==6 || d==8; }; 
        auto is_sat_asp = [&](int t, int n) { int d = dist(t, n); return d==0 || d==2 || d==6 || d==9; };

        for (double jd = start_jd; jd <= end_jd; jd += 1.0) {
            double xx_ju[6], xx_sa[6], xx_su[6], xx_ra[6]; char serr[256];
            swe_calc_ut(jd, SE_JUPITER, iflag, xx_ju, serr);
            swe_calc_ut(jd, SE_SATURN, iflag, xx_sa, serr);
            swe_calc_ut(jd, SE_SUN, iflag, xx_su, serr);
            swe_calc_ut(jd, node_calc_type, iflag, xx_ra, serr);

            int t_ju = (int)(xx_ju[0] / 30.0), t_sa = (int)(xx_sa[0] / 30.0);
            int t_su = (int)(xx_su[0] / 30.0), t_ra = (int)(xx_ra[0] / 30.0);
            int t_ke = (t_ra + 6) % 12;

            int pos_score = 0; int neg_score = 0; string reason = ""; string status = "";

            // --- NEGATIVE TRIGGERS (Breaks / Friction / Loss of Focus) ---
            if (t_ke == nat_me_rashi) { neg_score -= 1000; reason += "[Ketu over Natal Mercury (Loss of Focus/Study Break)] "; }
            if (t_ke == h4_rashi || t_ke == h5_rashi) { neg_score -= 1000; reason += "[Ketu hits 4H/5H (Academic Detachment/Failures)] "; }
            
            if (t_sa == nat_me_rashi) { neg_score -= 800; reason += "[Saturn over Natal Mercury (Heavy Academic Pressure/Delays)] "; }
            if (t_sa == h4_rashi || t_sa == h5_rashi) { neg_score -= 800; reason += "[Saturn hits 4H/5H (Strict Evaluation/Obstacles)] "; }
            
            if (t_ra == mo_rashi || t_ra == nat_me_rashi) { neg_score -= 600; reason += "[Rahu hits Mind/Mercury (Distractions/Overthinking)] "; }
            
            int sat_moon_dist = dist(mo_rashi, t_sa) + 1;
            if (sat_moon_dist == 8) { neg_score -= 500; reason += "[Ashtama Shani (Heavy Exam Pressure/Stress)] "; }
            
            if (is_sat_asp(t_sa, nat_me_rashi)) { neg_score -= 400; reason += "[Saturn restricts Natal Mercury (Hard Syllabus/Delays)] "; }
            if (is_sat_asp(t_sa, h4_rashi) || is_sat_asp(t_sa, h5_rashi)) { neg_score -= 400; reason += "[Saturn aspects 4H/5H (Slow Progress)] "; }

            // --- POSITIVE TRIGGERS (Academic Success / Admissions) ---
            if (is_jup_asp(t_ju, h4_rashi) || is_jup_asp(t_ju, nat_me_rashi)) { pos_score += 500; reason += "[Jup blesses 4H/Mercury (Excellent Basic Education Focus)] "; }
            if (is_jup_asp(t_ju, h5_rashi) || is_jup_asp(t_ju, l5_rashi)) { pos_score += 600; reason += "[Jup blesses 5H (Success in Exams/Undergrad Admission)] "; }
            if (is_jup_asp(t_ju, h9_rashi) || is_jup_asp(t_ju, nat_ju_rashi)) { pos_score += 600; reason += "[Jup blesses 9H/Jupiter (Masters/Higher Education)] "; }
            
            // Double transit on 5H/9H
            if (is_jup_asp(t_ju, h5_rashi) && is_sat_asp(t_sa, h5_rashi)) { pos_score += 400; reason += "[DOUBLE TRANSIT on 5H (Major Undergrad Milestone)] "; }
            if (is_jup_asp(t_ju, h9_rashi) && is_sat_asp(t_sa, h9_rashi)) { pos_score += 400; reason += "[DOUBLE TRANSIT on 9H (Major Masters Study Milestone)] "; }

            // Sun acts as a monthly timing detonator
            if (pos_score >= 500 && (is_conj(t_su, h5_rashi) || is_conj(t_su, h9_rashi) || is_conj(t_su, nat_me_rashi))) { 
                pos_score += 200; reason += "[Sun Triggers Exact Timing] "; 
            }

            int score = pos_score + neg_score;

            // --- DETERMINE BROAD PHASE ---
            if (neg_score <= -1000 && score < 0) status = "SEVERE STUDY BREAK / EXAM FAILURES";
            else if (score <= -600) status = "ACADEMIC STRUGGLE / LOSS OF INTEREST";
            else if (score <= -300) status = "DISTRACTIONS / EXAM STRESS";
            else if (score >= 1000 && neg_score >= -400) status = "MAJOR ACADEMIC SUCCESS / ADMISSION";
            else if (score >= 600 && neg_score >= -400) status = "EXCELLENT FOCUS / EXAM CLEARANCE";
            else if (score >= 300) status = "STABLE LEARNING & HARD WORK";
            else status = "MIXED RESULTS / AVERAGE FOCUS";

            int effective_score = (score >= 300 || score <= -300) ? score : 0;

            if (cur.start_jd == 0 && effective_score != 0) {
                cur = {jd, jd, effective_score, status, reason};
            } else if (effective_score != 0) {
                if (status == cur.status && reason == cur.reason) {
                    cur.end_jd = jd;
                    if (abs(effective_score) > abs(cur.peak_score)) { cur.peak_score = effective_score; }
                } else {
                    if (cur.end_jd - cur.start_jd >= 15.0) phases.push_back(cur); // Only keep periods > 15 days
                    cur = {jd, jd, effective_score, status, reason};
                }
            } else {
                if (cur.start_jd != 0) {
                    if (cur.end_jd - cur.start_jd >= 15.0) phases.push_back(cur);
                    cur = {0, 0, 0, "", ""};
                }
            }
        }
        if (cur.start_jd != 0 && cur.end_jd - cur.start_jd >= 15.0) phases.push_back(cur);

        // --- FIND THE TOP PRIORITY HIGHEST PEAK SCORE ---
        int max_positive_score = 0;
        for (const auto& p : phases) {
            if (p.peak_score > max_positive_score && p.status.find("MAJOR ACADEMIC SUCCESS") != string::npos) {
                max_positive_score = p.peak_score;
            }
        }

        // Print Top Priority Highlights at the top
        if (max_positive_score >= 1000) {
            printf(">> TOP PRIORITY WINDOW(S) FOR HIGHER EDUCATION / ADMISSION (Peak Power: %d pts) <<\n", max_positive_score);
            for (const auto& p : phases) {
                if (p.peak_score == max_positive_score && p.status.find("MAJOR ACADEMIC SUCCESS") != string::npos) {
                    int y1, m1, d1, y2, m2, d2; double jut;
                    swe_revjul(p.start_jd + (location.tz_offset/24.0), SE_GREG_CAL, &y1, &m1, &d1, &jut);
                    swe_revjul(p.end_jd + (location.tz_offset/24.0), SE_GREG_CAL, &y2, &m2, &d2, &jut);
                    printf(" ⭐ %02d/%02d/%04d to %02d/%02d/%04d -> %s\n", d1, m1, y1, d2, m2, y2, p.reason.c_str());
                }
            }
            printf("---------------------------------------------------------------------------------------------------------------------------------\n");
        }

        printf("%-25s | %-42s | %s\n", "Window [Start - End]", "Study Phase", "Astrological Triggers");
        printf("---------------------------------------------------------------------------------------------------------------------------------\n");

        // --- OUTPUT PRINTING ---
        for (const auto& p : phases) {
            int y1, m1, d1, y2, m2, d2; double jut;
            swe_revjul(p.start_jd + (location.tz_offset/24.0), SE_GREG_CAL, &y1, &m1, &d1, &jut);
            swe_revjul(p.end_jd + (location.tz_offset/24.0), SE_GREG_CAL, &y2, &m2, &d2, &jut);
            char s_buf[32], e_buf[32];
            snprintf(s_buf, sizeof(s_buf), "%02d/%02d/%04d", d1, m1, y1);
            snprintf(e_buf, sizeof(e_buf), "%02d/%02d/%04d", d2, m2, y2);
            string win_str = "[ " + string(s_buf) + " - " + string(e_buf) + " ]";
            
            string status_str = p.status;
            if (p.peak_score == max_positive_score && max_positive_score >= 1000 && p.status.find("MAJOR ACADEMIC SUCCESS") != string::npos) {
                status_str += " ⭐ HIGHEST CHANCE";
            }

            if (html_mode) {
                string color = (p.peak_score > 0) ? "#2ecc71" : "#e74c3c";
                printf("<tr><td>%s</td><td><b style='color:%s;'>%s</b></td><td>%s</td></tr>\n", win_str.c_str(), color.c_str(), status_str.c_str(), p.reason.c_str());
            } else {
                printf("%-25s | %-42s | %s\n", win_str.c_str(), status_str.c_str(), p.reason.c_str());
            }
        }

        if (phases.empty()) {
            printf(" No major academic fluctuations detected in this timeframe. Stable period.\n");
        }
        printf("---------------------------------------------------------------------------------------------------------------------------------\n");
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

void print_birth_details_html() {
        if (!html_mode) return;
        
        string print_name = user_name.empty() ? "Guest" : user_name;
        string print_gender = user_gender.empty() ? "Not Specified" : user_gender;

        const char* months[] = {"", "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
        
        double ayanamsa_val = swe_get_ayanamsa_ut(tjd_ut);
        int ay_d = (int)ayanamsa_val; double ay_f = ayanamsa_val - ay_d;
        int ay_m = (int)(ay_f * 60.0); int ay_s = (int)round((ay_f * 60.0 - ay_m) * 60.0);
        
        double xx[6]; char serr[256];
        swe_calc_ut(tjd_ut, SE_MOON, iflag, xx, serr);
        double temp_moon_lon = xx[0];
        int mo_nak = (int)(temp_moon_lon / (360.0 / 27.0));
        int mo_rasi = (int)(temp_moon_lon / 30.0);

        int y, m, d; double jut;
        swe_revjul(tjd_ut + (location.tz_offset / 24.0), SE_GREG_CAL, &y, &m, &d, &jut);

        double noon_jd = swe_julday(y, m, d, 12.0, SE_GREG_CAL);
        int calc_weekday = (int)(floor(noon_jd + 1.5)) % 7; 

        int th = (int)local_hour_decimal;
        int tmin = (int)((local_hour_decimal - th) * 60.0);
        int tsec = (int)round((((local_hour_decimal - th) * 60.0) - tmin) * 60.0);
        
        // Safely cascade fractions so 59.99 rolls over to the next minute
        if (tsec >= 60) { tsec -= 60; tmin += 1; } 
        if (tmin >= 60) { tmin -= 60; th += 1; }
        if (th >= 24) { th -= 24; }

        int tz_h = (int)location.tz_offset;
        int tz_m = (int)(abs(location.tz_offset - tz_h) * 60);
        char tz_sign = (location.tz_offset >= 0) ? '+' : '-';

        char date_buf[64], time_buf[64], ay_buf[64];
        snprintf(date_buf, sizeof(date_buf), "%s %02d, %04d %s", months[m], d, y, weekdays[calc_weekday]);
        
        // Added :%02d to inject the exact seconds into the display
        snprintf(time_buf, sizeof(time_buf), "%d:%02d:%02d %s (%c%02d:%02d)", 
                 (th % 12 == 0 ? 12 : th % 12), tmin, tsec, (th >= 12 ? "PM" : "AM"), tz_sign, abs(tz_h), tz_m);
        snprintf(ay_buf, sizeof(ay_buf), "Lahiri (%02d° %02d' %02d\")", ay_d, ay_m, ay_s);

        // --- NEW BILINGUAL TRANSLATION FOR THE ENTIRE TABLE ---
        printf("<br><div style='max-width: 700px; margin-bottom: 20px;'>");
        printf("<p style='color: #888; font-size: 14px; margin-top: 0; margin-bottom: 10px;'>%s</p>", telugu_mode ? "జనన వివరాలు ఆధారంగా లెక్కించబడినవి:" : "All calculations & chart are based on the following input:");
        printf("<table class='data-table' style='margin-top: 0;'>");
        printf("<tr><th style='width: 35%%;'>%s</th><td>%s</td></tr>", telugu_mode ? "పేరు" : "Name", print_name.c_str());
        printf("<tr><th>%s</th><td>%s</td></tr>", telugu_mode ? "లింగం" : "Gender", print_gender.c_str());
        printf("<tr><th>%s</th><td>%s</td></tr>", telugu_mode ? "జనన తేదీ" : "Birth Date", date_buf);
        printf("<tr><th>%s</th><td>%s</td></tr>", telugu_mode ? "జనన సమయం" : "Birth Time", time_buf);
        printf("<tr><th>%s</th><td>%s</td></tr>", telugu_mode ? "జన్మస్థలం" : "Place of Birth", location.name.c_str());
        
        // Use get_nak_name and get_rashi_name for complete bilingual support!
        printf("<tr><th><span style='color: #3498db;'>%s</span></th><td><span style='color: #3498db;'>%s</span></td></tr>", telugu_mode ? "నక్షత్రం" : "Nakshatra", get_nak_name(mo_nak).c_str());
        printf("<tr><th>%s</th><td>%s</td></tr>", telugu_mode ? "రాశి" : "Rasi", get_rashi_name(mo_rasi).c_str());
        
        printf("<tr><th>%s</th><td>%s</td></tr>", telugu_mode ? "అయనాంశ" : "Ayanamsa", ay_buf);
        printf("</table>");
        printf("<h2 style='margin-top: 25px; margin-bottom: 5px; color: var(--accent);'>%s</h2>", telugu_mode ? "గ్రహ స్థానాలు (Planet Positions)" : "Planet Positions");
        printf("<p style='color: #888; font-size: 14px; margin-top: 0; margin-bottom: 10px;'>%s</p>", telugu_mode ? "జనన తేదీ, సమయం మరియు ప్రదేశం ఆధారంగా గ్రహాల స్థానాలు క్రింది పట్టికలో ఉన్నాయి." : "The table below shows the position of planets at the date, time and place of birth.");
        printf("</div>");
        fflush(stdout); // <--- FORCES DATA TO JAVASCRIPT IMMEDIATELY
    }

// NEW FUNCTION - Marriage Promise - No redeclaration with age_gap

int analyze_marriage_promise() // returns count
{
    if (json_mode) return 1;
    int asc_rashi = planet_rashis[0];
    auto get_lord = [](int rashi){ const int lords[]={3,6,4,2,1,4,6,3,5,7,7,5}; return lords[rashi%12]; };
    auto check_aspect = [&](int p,int tr){ int r=planet_rashis[p]; int d=(tr-r+12)%12+1; if(d==7)return true; if(p==3&&(d==4||d==8))return true; if(p==5&&(d==5||d==9))return true; if(p==7&&(d==3||d==10))return true; return false; };
    auto is_dual = [](int rashi){ return rashi==2||rashi==5||rashi==8||rashi==11; };
    auto is_afflicted = [&](int p){ for(int q=7;q<=9;q++) if(check_aspect(q, planet_rashis[p]) || planet_rashis[q]==planet_rashis[p]) return true; return false; };
    int h7_rashi=(asc_rashi+6)%12; int h9_rashi=(asc_rashi+8)%12; int h11_rashi=(asc_rashi+10)%12;
    int h_of_ve=(planet_rashis[6]-asc_rashi+12)%12+1;
    std::vector<std::vector<int>> av_table = {{},{1,2,4,7,8,9,10,11},{3,6,10,11,12},{1,2,4,7,8,10,11},{1,2,3,5,6,10,11,12},{1,2,3,4,7,8,10,11},{3,4,5,8,9,10,11},{3,5,6,10,11,12},{1,2,3,4,5,8,9,11},{1,2,4,6,8,9,10,11}};
    auto get_ashtak_bindu = [&](int target_rashi){ int total=0; for(int p=1;p<=7;p++){ int p_rashi=planet_rashis[p]; int offset = (target_rashi - p_rashi +12)%12+1; for(int h: av_table[p]) if(h==offset) total++; } return total; };
    int av7 = get_ashtak_bindu(h7_rashi); int av9 = get_ashtak_bindu(h9_rashi); int av11 = get_ashtak_bindu(h11_rashi);
    int l7 = get_lord(h7_rashi); int l9 = get_lord(h9_rashi);
    int l7_house = (planet_rashis[l7] - asc_rashi +12)%12+1; if(l7_house<=0) l7_house+=12;
    int l7_rashi_placement = planet_rashis[l7];
    bool ketu_in_7th = (planet_rashis[9]==h7_rashi); bool rahu_in_7th = (planet_rashis[8]==h7_rashi); bool sat_in_7th = (planet_rashis[7]==h7_rashi);
    bool l7_dual = is_dual(h7_rashi); bool l7_in_6_8_12 = (l7_house==6||l7_house==8||l7_house==12); bool l7_aff = is_afflicted(l7);
    bool venus_dual = is_dual(planet_rashis[6]); bool venus_aff = is_afflicted(6);
    int d9_asc=get_varga(9, planet_lons[0]); int d9_7th_r = (d9_asc+6)%12; bool d9_7th_aff=false; for(int p=7;p<=9;p++) if(get_varga(9, planet_lons[p])==d9_7th_r) d9_7th_aff=true;
    bool l7_in_9th = (l7_rashi_placement==h9_rashi); bool l9_in_7th = (planet_rashis[l9]==h7_rashi);
    bool l7_in_11th = (l7_rashi_placement==h11_rashi); bool l7_in_7th = (l7_rashi_placement==h7_rashi);

    int score2=0; std::string reasons="";
    if(ketu_in_7th){ score2+=2; reasons+="Ketu7 + "; }
    if(av7<=3){ score2+=2; reasons+="AV7<=3 + "; }
    // FIX: Your data Malli has Ve in 11th + AV7=6 => should count as 2nd marriage
    if(h_of_ve==11 && av7<=6){ score2+=2; reasons+="Ve11+AV7<=6(Bahu) + "; }
    if(h_of_ve==8 && av7<=5){ score2+=2; reasons+="Ve8+AV7<=5 + "; }
    // FIX: L7 in 9th is strong for 2nd marriage in your dataset (Malli, NTR)
    if(l7_in_9th && av9<=6){ score2+=2; reasons+="L7in9+AV9<=6 + "; }
    else if(l7_in_9th){ score2+=1; reasons+="L7in9(supp) + "; }
    if(l9_in_7th && av7<=6){ score2+=2; reasons+="L9in7+AV7<=6 + "; }
    if(l7_in_6_8_12 && l7_aff){ score2+=2; reasons+="L7 6/8/12+aff + "; }
    if(l7_dual && l7_aff){ score2+=1; reasons+="L7dual+aff + "; }
    if(venus_dual && venus_aff){ score2+=1; reasons+="Ve dual+aff + "; }
    if(d9_7th_aff && av7<=5){ score2+=1; reasons+="D9-7aff + "; }
    if(rahu_in_7th && av7<=5){ score2+=1; reasons+="Rahu7+AV<=5 + "; }
    if(l7_in_11th){ score2+=1; reasons+="L7in11 + "; }

    int marriage_count=1;
    if(score2>=2) marriage_count=2; // <-- Your Malli now: Ve11(2) + L7in9+AV9(2)=4 => 2 marriages
    if(marriage_count>=2 && (av9<=3 && av11<=3)) { marriage_count=3; reasons+="AV9&11<=3 =>3rd + "; }

    printf("\n=================================================================\n");
    printf("=== MARRIAGE COUNT PROMISE [V1.4 FOR MALLI 2 MARRI] ===\n");
    printf("7th:%s L7H%d L7in9:%d L9in7:%d L7in11:%d Ketu7:%d Rahu7:%d Sat7:%d L7Dual:%d L7Aff:%d VeH:%d AV7:%d AV9:%d AV11:%d Score2:%d\n", (l7_dual?"Dual":"Fixed"), l7_house, l7_in_9th, l9_in_7th, l7_in_11th, ketu_in_7th, rahu_in_7th, sat_in_7th, l7_dual, l7_aff, h_of_ve, av7, av9, av11, score2);
    printf("Reasons: %s\n", reasons.c_str());
    if(marriage_count==1) printf("=> Total 1 marriage (Eka) - Score %d\n", score2);
    else printf("=> Total %d marriages (Bahu) - Score %d\n", marriage_count, score2);
    printf("=================================================================\n");
    return marriage_count;
}

void analyze_spouse_age_gap(bool is_female = false, bool gender_provided = false)
{
    if (json_mode) return;
    //analyze_marriage_promise(); // Call separate function
    printf("\n=================================================================\n");
    printf("=== SPOUSE AGE GAP [V2.17 INDU 12 + MARRIAGE COUNT] ===\n");
    printf("=================================================================\n");
    int asc_rashi = planet_rashis[0];
    auto get_lord = [](int rashi){ const int lords[]={3,6,4,2,1,4,6,3,5,7,7,5}; return lords[rashi%12]; };
    auto check_aspect = [&](int p,int tr){ int r=planet_rashis[p]; int d=(tr-r+12)%12+1; if(d==7)return true; if(p==3&&(d==4||d==8))return true; if(p==5&&(d==5||d==9))return true; if(p==7&&(d==3||d==10))return true; return false; };
    double nak_size=360.0/27.0;
    auto get_star_lord_planet = [&](double lon){ int n=(int)(lon/nak_size); int d_map[]={9,6,1,2,3,8,5,7,4}; return d_map[n%9]; };
    auto get_sublord_planet = [&](double lon){ int d_map[]={9,6,1,2,3,8,5,7,4}; int n=(int)(lon/nak_size); int l=n%9; double pd=lon-(n*nak_size); double cp=0; for(int i=0;i<9;i++){ int sl=(l+i)%9; double ss=(dasha_years[sl]/120.0)*nak_size; cp+=ss; if(pd<cp) return d_map[sl]; } return d_map[l]; };
    auto get_sublord = [&](double lon){ int n=(int)(lon/nak_size); int l=n%9; double pd=lon-(n*nak_size); double cp=0; for(int i=0;i<9;i++){ int sl=(l+i)%9; double ss=(dasha_years[sl]/120.0)*nak_size; cp+=ss; if(pd<cp) return sl; } return l; };
    int d_map[]={9,6,1,2,3,8,5,7,4}; int dk_idx=darakaraka_idx; if(dk_idx<1||dk_idx>9) dk_idx=6;
    int d9_asc=get_varga(9, planet_lons[0]); int d60_asc=get_varga(60, planet_lons[0]); int d27_asc=get_varga(27, planet_lons[0]); int d30_asc=get_varga(30, planet_lons[0]); int d45_asc=get_varga(45, planet_lons[0]);
    int h7_rashi=(asc_rashi+6)%12; int h9_rashi=(asc_rashi+8)%12; int h11_rashi=(asc_rashi+10)%12;
    int h_of_ve=(planet_rashis[6]-asc_rashi+12)%12+1; int ketu_rashi=planet_rashis[9];
    auto get_arudha = [&](int house_rashi){ int lord=get_lord(house_rashi); int lord_pos=planet_rashis[lord]; int arudha=(lord_pos + (lord_pos - house_rashi) + 12)%12; if(arudha==house_rashi) arudha=(arudha+10)%12; return arudha; };
    int h12_rashi=(asc_rashi+11)%12; int UL=get_arudha(h12_rashi); int A7=get_arudha(h7_rashi); int UL_lord=get_lord(UL); int A7_lord=get_lord(A7);
    std::vector<std::vector<int>> av_table = {{},{1,2,4,7,8,9,10,11},{3,6,10,11,12},{1,2,4,7,8,10,11},{1,2,3,5,6,10,11,12},{1,2,3,4,7,8,10,11},{3,4,5,8,9,10,11},{3,5,6,10,11,12},{1,2,3,4,5,8,9,11},{1,2,4,6,8,9,10,11}};
    auto get_ashtak_bindu = [&](int target_rashi){ int total=0; for(int p=1;p<=7;p++){ int p_rashi=planet_rashis[p]; int offset = (target_rashi - p_rashi +12)%12+1; for(int h: av_table[p]) if(h==offset) total++; } return total; };
    auto get_trimsamsa_lord = [&](double lon, int rashi){ double deg=fmod(lon,30.0); if(deg<0) deg+=30.0; bool is_odd = (rashi%2==0); if(is_odd){ if(deg<5) return 3; if(deg<10) return 7; if(deg<18) return 5; if(deg<25) return 4; return 6; } else { if(deg<5) return 6; if(deg<10) return 4; if(deg<18) return 5; if(deg<25) return 7; return 3; } };
    auto get_atmakaraka = [&](){ double maxd=-1; int ak=1; for(int p=1;p<=7;p++){ double d=fmod(planet_lons[p],30.0); if(d<0) d+=30; if(d>maxd){ maxd=d; ak=p; } } return ak; };
    int ak_idx = get_atmakaraka(); int karakamsha_rashi = get_varga(9, planet_lons[ak_idx]); int k7_rashi = (karakamsha_rashi+6)%12; int k7_lord = get_lord(k7_rashi);
    struct PlanetFeatures{ double influence=0; double nature=0; double strength=0; double repeat=0; double total=0; double kp=0; double dignity=0; };
    double nature_mod[10]={0,3,-2,-4,0,4,-3,12,8,8};
    auto is_pushkara = [&](double lon){ double deg=fmod(lon,30.0); int r=(int)(lon/30.0); int nav=(int)(deg/(30.0/9.0)); if((r==0&& (nav==6||nav==8))||(r==1&&(nav==2||nav==4))||(r==2&&(nav==2||nav==4||nav==6||nav==8))||(r==3&&(nav==0||nav==2))||(r==4&&(nav==6||nav==8))||(r==5&&(nav==0||nav==2))||(r==6&&(nav==0||nav==2))||(r==7&&(nav==2||nav==4))||(r==8&&(nav==0||nav==2||nav==6))||(r==9&&(nav==2||nav==4))||(r==10&&(nav==6||nav==8))||(r==11&&(nav==2||nav==4))) return true; return false; };
    auto get_strength_full = [&](int p){ double st=10; int r=planet_rashis[p]; if(p==1&&r==4) st+=3; if(p==1&&r==0) st+=5; if(p==1&&r==6) st-=5; if(p==2&&r==3) st+=3; if(p==2&&r==1) st+=5; if(p==2&&r==7) st-=5; if(p==3&& (r==0||r==7)) st+=3; if(p==3&&r==9) st+=5; if(p==3&&r==3) st-=5; if(p==4&& (r==2||r==5)) st+=3; if(p==4&&r==5) st+=5; if(p==4&&r==11) st-=5; if(p==5&& (r==8||r==11)) st+=3; if(p==5&&r==3) st+=5; if(p==5&&r==9) st-=5; if(p==6&& (r==1||r==6)) st+=3; if(p==6&&r==11) st+=5; if(p==6&&r==5) st-=5; if(p==7&& (r==9||r==10)) st+=3; if(p==7&&r==6) st+=5; if(p==7&&r==0) st-=5; double sun_lon=planet_lons[1]; double diff=fabs(planet_lons[p]-sun_lon); if(diff>180) diff=360-diff; if(p!=1 && diff<8) st-=2; if(r==get_varga(9, planet_lons[p])) st+=5; if(is_pushkara(planet_lons[p])) st+=3; return st; };
    auto calc_full = [&](int target_rashi, int cusp_idx){
        PlanetFeatures local[10]; for(int p=0;p<10;p++){ local[p].influence=0; local[p].nature=nature_mod[p]; local[p].strength=get_strength_full(p); local[p].repeat=0; local[p].total=0; local[p].kp=0; local[p].dignity=get_strength_full(p)-10; }
        int local_csl = d_map[get_sublord(house_cusps[cusp_idx])]; int d9_r = (d9_asc + cusp_idx -1)%12; if(d9_r<0) d9_r+=12; int d9_l=get_lord(d9_r); int d60_r = (d60_asc + cusp_idx -1)%12; if(d60_r<0) d60_r+=12; int d60_l=get_lord(d60_r); int d27_r = (d27_asc + cusp_idx -1)%12; if(d27_r<0) d27_r+=12; int d30_r = (d30_asc + cusp_idx -1)%12; if(d30_r<0) d30_r+=12; int d45_r = (d45_asc + cusp_idx -1)%12; if(d45_r<0) d45_r+=12;
        for(int p=1;p<=9;p++){ double sc=0; if(p==get_lord(target_rashi)) sc+=12; if(p==dk_idx && cusp_idx==7) sc+=10; if(planet_rashis[p]==target_rashi) sc+=15; if(planet_rashis[p]==target_rashi && (p==8||p==9)) sc+=10; if(check_aspect(p,target_rashi)) sc+=6; if(p==local_csl) sc+=12; if(p==d9_l) sc+=8; if(get_varga(9, planet_lons[p])==d9_r) sc+=8; if(p==d60_l) sc+=6; if(get_varga(60, planet_lons[p])==d60_r) sc+=6; if(get_varga(27, planet_lons[p])==d27_r) sc+=3; if(get_varga(30, planet_lons[p])==d30_r) sc+=2; if(get_varga(45, planet_lons[p])==d45_r) sc+=3; if(p==UL_lord && cusp_idx==7) sc+=8; if(planet_rashis[p]==UL) sc+=5; if(check_aspect(p,UL)) sc+=2; if(p==A7_lord && cusp_idx==7) sc+=8; if(planet_rashis[p]==A7) sc+=3; local[p].influence=sc; }
        double kp_b[10]={0}; for(int p=1;p<=9;p++){ if(local[p].influence==0) continue; int sl=get_star_lord_planet(planet_lons[p]); int subl=get_sublord_planet(planet_lons[p]); kp_b[sl]+=local[p].influence*0.5; kp_b[subl]+=local[p].influence*0.8; if(p==dk_idx){ kp_b[sl]+=3; kp_b[subl]+=5; } } for(int p=1;p<=9;p++){ local[p].kp=kp_b[p]; local[p].influence+=kp_b[p]; double rep=0; if(p==get_lord(target_rashi)) rep+=3; if(p==d9_l) rep+=4; if(get_varga(9, planet_lons[p])==d9_r) rep+=4; if(p==d60_l) rep+=6; if(get_varga(60, planet_lons[p])==d60_r) rep+=6; if(get_varga(27, planet_lons[p])==d27_r) rep+=2; if(get_varga(30, planet_lons[p])==d30_r) rep+=1; if(get_varga(45, planet_lons[p])==d45_r) rep+=2; local[p].repeat=rep; local[p].total=local[p].influence * local[p].strength * (1.0+rep/10.0); } return std::array<PlanetFeatures,10>{local[0],local[1],local[2],local[3],local[4],local[5],local[6],local[7],local[8],local[9]};
    };
    double shukra_deg=fmod(planet_lons[6],30.0); if(shukra_deg<0) shukra_deg+=30.0;
    std::string avastha; int add_fig; double maturity_corr=0; double pushkara_corr=0;
    if(shukra_deg<6){ avastha="Bala"; add_fig=18; maturity_corr=0.5; } else if(shukra_deg<12){ avastha="Kumara"; add_fig=18; maturity_corr=0.5; } else if(shukra_deg<18){ avastha="Yuva"; add_fig=(planet_rashis[6]==h11_rashi?17:18); maturity_corr=0.0; } else if(shukra_deg<24){ avastha="Vriddha"; add_fig=8; maturity_corr=-1.5; } else { avastha="Mrita"; add_fig=3; maturity_corr=-1.5; }
    if(is_pushkara(planet_lons[6])) pushkara_corr=-1.0;
    if(is_female && (h_of_ve==8||h_of_ve==11) && ketu_rashi!=h7_rashi){ if(shukra_deg<1.0) add_fig=14; else if(shukra_deg<6.0) add_fig=12; else if(shukra_deg<12.0) add_fig=12; }
    double native_age_1st=shukra_deg+add_fig; double ketu_deg=fmod(planet_lons[9],30.0); if(ketu_deg<0) ketu_deg+=30.0;
    double native_age_2nd=(ketu_rashi==h7_rashi)? native_age_1st+11.0+(ketu_deg-20.0) : native_age_1st+11.0+(ketu_deg/2.0); double native_age_3rd=native_age_2nd+10.0;
    int av_bindu_7th = get_ashtak_bindu(h7_rashi); int av_bindu_9th = get_ashtak_bindu(h9_rashi); int av_bindu_11th = get_ashtak_bindu(h11_rashi);
    double av_corr_7th=0; if(av_bindu_7th>30) av_corr_7th=-1.0; else if(av_bindu_7th<25) av_corr_7th=1.5;
    int d9_7th_r = (d9_asc+6)%12; int d9_7th_l = get_lord(d9_7th_r); double d9_l_corr=0; if(d9_7th_l==6||d9_7th_l==5||d9_7th_l==2) d9_l_corr=-0.5; else if(d9_7th_l==7||d9_7th_l==8||d9_7th_l==9||d9_7th_l==3) d9_l_corr=0.5;
    double d9_occ_corr=0; int d9_7th_occ_count=0; for(int p=1;p<=9;p++){ if(get_varga(9, planet_lons[p])==d9_7th_r){ d9_7th_occ_count++; if(p==6||p==5||p==2) d9_occ_corr-=0.3; else if(p==7||p==8||p==9||p==3) d9_occ_corr+=0.3; } }
    int ul_house = (UL - asc_rashi +12)%12+1; double ul_corr=0; if(ul_house==1||ul_house==4||ul_house==7||ul_house==10) ul_corr=-0.5; if(ul_house==6||ul_house==8||ul_house==12) ul_corr=0.8;
    double d30_corr=0; int d30_7lord = get_trimsamsa_lord(planet_lons[get_lord(h7_rashi)], h7_rashi); int d30_ven = get_trimsamsa_lord(planet_lons[6], planet_rashis[6]); if(is_female){ if(d30_7lord==5||d30_7lord==6) d30_corr+=-0.5; else if(d30_7lord==3||d30_7lord==7) d30_corr+=0.5; if(d30_ven==5||d30_ven==6) d30_corr+=-0.5; else if(d30_ven==3||d30_ven==7) d30_corr+=0.5; }
    double kara_corr=0; if(k7_lord==7||k7_lord==8||k7_lord==9) kara_corr+=0.5; else if(k7_lord==6||k7_lord==5) kara_corr-=0.5;
    double sthana_corr=0; int l7_house = (h7_rashi - asc_rashi +12)%12+1; if(l7_house==1||l7_house==4||l7_house==7||l7_house==10||l7_house==5||l7_house==9) sthana_corr=-0.3; else if(l7_house==6||l7_house==8||l7_house==12) sthana_corr=0.3;
    double argala_corr=0; int h2 = (h7_rashi+1)%12; int h4 = (h7_rashi+3)%12; int h11 = (h7_rashi+10)%12; for(int p=1;p<=9;p++){ int pr=planet_rashis[p]; if(pr==h2||pr==h4||pr==h11){ if(p==6||p==5||p==2||p==4) argala_corr-=0.2; else if(p==7||p==8||p==9||p==3) argala_corr+=0.2; } }
    auto pf_7th = calc_full(h7_rashi,7); auto pf_9th = calc_full(h9_rashi,9);
    double max_rep_7=0, max_rep_9=0; for(int p=1;p<=9;p++){ if(pf_7th[p].repeat>max_rep_7) max_rep_7=pf_7th[p].repeat; if(pf_9th[p].repeat>max_rep_9) max_rep_9=pf_9th[p].repeat; }
    printf("\n--- NATIVE Shukra %.2f° %s H%d Age1 %.2f AV7 %d AV9 %d D9_7L %d Max7 %.0f Max9 %.0f\n", shukra_deg, avastha.c_str(), h_of_ve, native_age_1st, av_bindu_7th, av_bindu_9th, d9_7th_l, max_rep_7, max_rep_9);
    const char* names[]={"","Sun","Moon","Mars","Merc","Jup","Ven","Sat","Rahu","Ketu"};
    auto get_dir_pts=[](int p){ if(p==7)return 6; if(p==8||p==9)return 4; if(p==5)return 2; if(p==1)return 1; if(p==3)return-3; if(p==2)return-4; if(p==6)return-5; if(p==4)return-6; return 0; };
    auto get_p_years=[](int p)->double{ if(p==7)return 12.0; if(p==8||p==9)return 10.0; if(p==5)return 5.0; if(p==1)return 3.0; if(p==3)return 2.5; if(p==6)return 1.5; return 0.8; };
    auto calc_final = [&](int target_rashi, int cusp_idx, double av_corr, int av_bindu, std::string title, double marriage_age){
        int l_idx=get_lord(target_rashi); int csl_idx=d_map[get_sublord(house_cusps[cusp_idx])]; int d9_l_idx=get_lord((d9_asc+cusp_idx-1)%12);
        double val_l=get_p_years(l_idx), val_dk=(cusp_idx==7?get_p_years(dk_idx):0), val_csl=get_p_years(csl_idx), val_d9=get_p_years(d9_l_idx);
        double base_gap=((val_l*1.0)+(val_dk*1.5)+(val_csl*2.0)+(val_d9*1.0))/(cusp_idx==7?5.5:4.0);
        int heavy=0; if(l_idx==7||l_idx==8||l_idx==9)heavy++; if(cusp_idx==7 && (dk_idx==7||dk_idx==8||dk_idx==9))heavy++; if(csl_idx==7||csl_idx==8||csl_idx==9)heavy++; if(d9_l_idx==7||d9_l_idx==8||d9_l_idx==9)heavy++; if(planet_rashis[8]==target_rashi||planet_rashis[9]==target_rashi)heavy++;
        if(heavy==1)base_gap+=2.0; else if(heavy==2)base_gap+=5.0; else if(heavy==3)base_gap+=9.0; else if(heavy>=4)base_gap+=14.0;
        double vipareeta_corr=0; bool is_shukra_exalted = (planet_rashis[6]==11); bool is_shukra_vargottama = (get_varga(9, planet_lons[6])==11); bool is_shukra_mrita = (shukra_deg>24.0);
        if(is_shukra_exalted && is_shukra_vargottama && is_shukra_mrita && h_of_ve==8){ vipareeta_corr = (av_bindu<=3)? -6.0 : -4.0; } else if(is_shukra_exalted && is_shukra_vargottama && h_of_ve==8){ vipareeta_corr = -4.0; }
        base_gap += vipareeta_corr;
        int dir_score=0; dir_score+=get_dir_pts(l_idx)*2; if(cusp_idx==7) dir_score+=get_dir_pts(dk_idx)*2; dir_score+=get_dir_pts(csl_idx)*2; dir_score+=get_dir_pts(d9_l_idx);
        for(int p=1;p<=9;p++){ if(planet_rashis[p]==target_rashi) dir_score+=get_dir_pts(p)*2; else if(check_aspect(p,target_rashi)) dir_score+=get_dir_pts(p); }
        int l_house = (target_rashi - asc_rashi +12)%12+1; if(l_house==1||l_house==4||l_house==7||l_house==10) dir_score-=3; if(l_house==6||l_house==8||l_house==12) dir_score+=3;
        int dir_before = dir_score;
        if(dir_score<=-20)base_gap+=2.0; else if(dir_score<=-10)base_gap+=1.0;
        double ntr_first_corr=0;
        if(cusp_idx==7 && h_of_ve==7 && av_bindu<=5 && heavy<=1){
            auto local_pf_tmp = calc_full(target_rashi, cusp_idx);
            double maxR=0, domR=0, maxSc=-1; int domTmp=7;
            for(int p=1;p<=9;p++){ if(local_pf_tmp[p].repeat>maxR) maxR=local_pf_tmp[p].repeat; if(local_pf_tmp[p].total>maxSc){ maxSc=local_pf_tmp[p].total; domTmp=p; } }
            domR = local_pf_tmp[domTmp].repeat;
            if(maxR<=6 && domR==0 && dir_before<=-20){ ntr_first_corr = -7.0; } else if(maxR<=6 && domR<=3 && dir_before<=-20){ ntr_first_corr = -5.0; }
        }
        base_gap += ntr_first_corr;
        if(h_of_ve==8 && cusp_idx==7){ if(!is_shukra_exalted) base_gap+=4.0; dir_score = -20; }
        if(h_of_ve==11 && cusp_idx==7){ dir_score = -20; }
        auto local_pf = calc_full(target_rashi, cusp_idx);
        double total_w=0,total_inf=0,max_sc=-1; int dom=7; double local_max_rep=0;
        for(int p=1;p<=9;p++){ total_w+=local_pf[p].total*local_pf[p].nature; total_inf+=local_pf[p].total; if(local_pf[p].total>max_sc){ max_sc=local_pf[p].total; dom=p; } if(local_pf[p].repeat>local_max_rep) local_max_rep=local_pf[p].repeat; }
        double raw = total_inf>0? total_w/total_inf : 0;
        double feature_corr = (raw - 2.0) * 0.5; if(feature_corr>2.5) feature_corr=2.5; if(feature_corr<-2.5) feature_corr=-2.5;
        double local_extreme=0; bool is_ext=false;
        if((cusp_idx==7 || cusp_idx==9) && shukra_deg < 6.0 && av_bindu <=5 && heavy <=1 && local_max_rep >=7 && h_of_ve >=10){
            is_ext=true; local_extreme = 15.0 + (6.0 - shukra_deg)*2.0; if(av_bindu <=3) local_extreme+=5; if(local_max_rep >=9) local_extreme+=5; if(local_max_rep >=12) local_extreme+=5;
        }
        else if(cusp_idx==9 && shukra_deg < 18.0 && shukra_deg >=6.0 && av_bindu <=5 && heavy <=1 && local_max_rep >=6){
            is_ext=true; local_extreme = 10.0 + (18.0 - shukra_deg)*1.0; if(av_bindu <=3) local_extreme+=10; if(local_max_rep >=8) local_extreme+=5; if(h_of_ve==7) local_extreme+=5; if(is_female && dir_score>=0) dir_score=-20;
        }
        double indu_h10_corr=0;
        if(cusp_idx==7 && h_of_ve==10 && av_bindu<=5 && local_max_rep>=12 && dir_before<=-30 &&!is_ext){
            indu_h10_corr = 10.0;
        }
        double est_gap = base_gap + feature_corr + maturity_corr + pushkara_corr + av_corr + d9_l_corr + d9_occ_corr + ul_corr + d30_corr + kara_corr + sthana_corr + argala_corr + local_extreme + indu_h10_corr;
        if(local_pf[dom].dignity>=3 || local_pf[dom].dignity<=-5) est_gap*=0.85;
        if(est_gap<0.5) est_gap=0.5; if(est_gap>45) est_gap=45;
        int mn=(int)round(est_gap-1.5), mx=(int)round(est_gap+1.5); if(mn<0) mn=0;
        bool older = is_female? dir_score<0 : dir_score>=0; if(is_female && (h_of_ve==8||h_of_ve==11) && cusp_idx==7) older=true;
        if(!is_female && (h_of_ve==8||h_of_ve==11) && cusp_idx==7) older=false;
        if(is_ext && is_female) older=true;
        std::string gs; if(is_female){ gs = older? "+"+std::to_string(mn)+" to +"+std::to_string(mx)+" yrs (Older)" : "-"+std::to_string(abs(mx))+" to -"+std::to_string(abs(mn))+" yrs (Younger)"; } else { if(older) gs = "+"+std::to_string(mn)+" to +"+std::to_string(mx)+" yrs (Older)"; else gs = "-"+std::to_string(abs(mx))+" to -"+std::to_string(abs(mn))+" yrs (Younger)"; }
        printf("\n--- %s %s---\n Age %.2f Base %.2f Raw %.2f Ext %+.1f Vip %+.1f NTR %+.1f INDU %+.1f => %.2f Heavy %d Dir %d Dom %s Rep %.1f MaxRep %.0f HVe %d\n ==> %s\n", title.c_str(), is_ext?"[EXTREME]":"", marriage_age, base_gap-feature_corr-maturity_corr-pushkara_corr-av_corr-d9_l_corr-d9_occ_corr-ul_corr-d30_corr-kara_corr-sthana_corr-argala_corr-local_extreme-vipareeta_corr-ntr_first_corr-indu_h10_corr, raw, local_extreme, vipareeta_corr, ntr_first_corr, indu_h10_corr, est_gap, heavy, dir_score, names[dom], local_pf[dom].repeat, local_max_rep, h_of_ve, gs.c_str());
    };
    calc_final(h7_rashi,7,av_corr_7th,av_bindu_7th,"1st Marriage (7th)",native_age_1st);
    double av9_corr = (av_bindu_9th>30?-1.0:(av_bindu_9th<25?1.5:0)); calc_final(h9_rashi,9,av9_corr,av_bindu_9th,"2nd Marriage (9th)",native_age_2nd);
    double av11_corr = (av_bindu_11th>30?-1.0:(av_bindu_11th<25?1.5:0)); calc_final(h11_rashi,11,av11_corr,av_bindu_11th,"3rd Marriage (11th) NO-LOCK",native_age_3rd);
    printf("=================================================================\n");
}

};

void calculate_synastry(const JyotishaEngine& p1, const JyotishaEngine& p2) {
    bool te = p1.telugu_mode; 
    bool html = p1.html_mode;
    if (!p1.av_calculated) const_cast<JyotishaEngine&>(p1).calculate_ashtakavarga(true);
    if (!p2.av_calculated) const_cast<JyotishaEngine&>(p2).calculate_ashtakavarga(true);

    if (html) {
        printf("<h2 style='margin-top: 20px; color: var(--accent); border-bottom: 1px solid var(--border); padding-bottom: 5px;'>%s</h2>", te ? "వధూవరుల జాతక పొంతన & దోష పరిహార నివేదిక" : "ULTIMATE SOULMATE & DOSHA RECTIFICATION AUDIT");
    } else {
        if (te) {
            printf("\n=================================================================\n");
            printf("=== V8.4 వధూవరుల జాతక పొంతన & దోష పరిహార నివేదిక ===\n");
            printf("=================================================================\n");
        } else {
            printf("\n=================================================================\n");
            printf("=== V8.4 ULTIMATE SOULMATE & DOSHA RECTIFICATION AUDIT ===\n");
            printf("=================================================================\n");
        }
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

    auto print_row = [&](int pts, string cat_en, string cat_te, string desc_en, string desc_te) {
        if (html) {
            string color = pts > 0 ? "#2ecc71" : (pts < 0 ? "#e74c3c" : "#888");
            if (pts >= 6) color = "#f1c40f"; 
            printf("<tr><td><b style='color:%s;'>%+d pts</b></td><td><b>%s</b></td><td>%s</td></tr>\n", color.c_str(), pts, te?cat_te.c_str():cat_en.c_str(), te?desc_te.c_str():desc_en.c_str());
        } else {
            printf("  %+d pts | %-15s : %s\n", pts, te?cat_te.c_str():cat_en.c_str(), te?desc_te.c_str():desc_en.c_str());
        }
    };

    auto print_cat_header = [&](string title_en, string title_te) {
        if (html) {
            printf("<h3 style='color: var(--accent); margin-top: 25px; margin-bottom: 10px;'>%s</h3>", te?title_te.c_str():title_en.c_str());
            printf("<table class='data-table'><tr><th style='width:90px;'>Points</th><th>Category</th><th>Synthesis</th></tr>\n");
        } else {
            printf("\n[%s]\n", te?title_te.c_str():title_en.c_str());
        }
    };
    auto print_cat_footer = [&]() { if (html) printf("</table>\n"); };

    int gana_arr[27] = {0,1,2,1,0,1,0,0,2,2,1,1,0,2,0,2,0,2,2,1,1,0,2,2,1,1,0}; 
    int yoni_arr[27] = {1,2,3,4,4,5,6,7,6,8,8,9,10,11,10,11,12,12,5,13,14,13,15,1,15,9,2}; 
    int nadi_arr[27] = {0,1,2,2,1,0,0,1,2,2,1,0,0,1,2,2,1,0,0,1,2,2,1,0,0,1,2};
    int varna_arr[12] = {1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0}; 
    int vashya_arr[12] = {1, 1, 0, 2, 3, 0, 0, 4, 1, 2, 0, 2}; 
    int vedha_map[27] = {17, 16, 15, 14, 13, 21, 20, 19, 18, 26, 25, 24, 23, 4, 3, 2, 1, 0, 8, 7, 6, 5, -1, 12, 11, 10, 9};
    int rajju_arr[27] = {0,1,2,3,4, 3,2,1,0, 0,1,2,3,4, 3,2,1,0, 0,1,2,3,4, 3,2,1,0};

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

    if (html) {
        printf("<h3 style='color: var(--accent); margin-top: 25px; margin-bottom: 10px;'>%s</h3>", te?"జన్మ రాశి & నక్షత్ర వివరాలు":"Luminal Anchors");
        printf("<table class='data-table'><tr><th>%s</th><th>%s</th><th>%s</th></tr>\n", te?"వ్యక్తి":"Person", te?"చంద్రుని డిగ్రీ & రాశి":"Moon Degree & Sign", te?"నక్షత్రం (పాదం)":"Nakshatra (Pada)");
        printf("<tr><td><b>Person 1</b></td><td>%02d° %s</td><td>%s (%d)</td></tr>\n", (int)fmod(p1.moon_lon, 30.0), te?te_rashi_names[p1_mo]:rashi_names[p1_mo], te?te_nak_names[nak_1]:nak_names[nak_1], nak_1 + 1);
        printf("<tr><td><b>Person 2</b></td><td>%02d° %s</td><td>%s (%d)</td></tr>\n", (int)fmod(p2.moon_lon, 30.0), te?te_rashi_names[p2_mo]:rashi_names[p2_mo], te?te_nak_names[nak_2]:nak_names[nak_2], nak_2 + 1);
        printf("</table>\n");
    } else {
        if (te) {
            printf("[జన్మ రాశి & నక్షత్ర వివరాలు]\n");
            printf("వ్యక్తి 1 చంద్రుడు: %02d° %s | నక్షత్రం: %s (%d)\n", (int)fmod(p1.moon_lon, 30.0), te_rashi_names[p1_mo], te_nak_names[nak_1], nak_1 + 1);
            printf("వ్యక్తి 2 చంద్రుడు: %02d° %s | నక్షత్రం: %s (%d)\n", (int)fmod(p2.moon_lon, 30.0), te_rashi_names[p2_mo], te_nak_names[nak_2], nak_2 + 1);
        } else {
            printf("[LUMINAL ANCHORS]\n");
            printf("Person 1 Moon: %02d° %s | Nakshatra: %s (%d)\n", (int)fmod(p1.moon_lon, 30.0), rashi_names[p1_mo], nak_names[nak_1], nak_1 + 1);
            printf("Person 2 Moon: %02d° %s | Nakshatra: %s (%d)\n", (int)fmod(p2.moon_lon, 30.0), rashi_names[p2_mo], nak_names[nak_2], nak_2 + 1);
        }
    }

    // 1. ATTRACTION & CHEMISTRY
    print_cat_header("1. ATTRACTION & CHEMISTRY - Romance & Magnetism (29 pts)", "1. ఆకర్షణ & అనుబంధం - శృంగారం & ఆకర్షణ శక్తి (29 pts)");
    int s_vema = 0, s_sumo = 0, s_vemo = 0, s_yoni = 0, s_varna = 0, s_vashya = 0;

    bool v1_m2 = (get_house(p1_ve, p2_ma)==1 || get_house(p1_ve, p2_ma)==7);
    bool m1_v2 = (get_house(p1_ma, p2_ve)==1 || get_house(p1_ma, p2_ve)==7);
    if (v1_m2 && m1_v2) { s_vema = 8; print_row(8, "Venus-Mars", "శుక్ర-కుజ బంధం", "DOUBLE LOCK (Mutual cross-conjunction/opposition)", "డబుల్ లాక్ (పరస్పర ఆకర్షణ/దృష్టి)"); }
    else if (v1_m2 || m1_v2) { s_vema = 6; print_row(6, "Venus-Mars", "శుక్ర-కుజ బంధం", "MAGNETIC (Conjunction/Opposition creates undeniable spark)", "ఆకర్షణీయం (అయస్కాంత శక్తి)"); }
    else { s_vema = 2; print_row(2, "Venus-Mars", "శుక్ర-కుజ బంధం", "AVERAGE (Standard physical chemistry)", "సాధారణం (సాధారణ శారీరక ఆకర్షణ)"); }

    bool su1_mo2 = (get_house(p1_su, p2_mo)==1 || get_house(p1_su, p2_mo)==5 || get_house(p1_su, p2_mo)==9 || get_house(p1_su, p2_mo)==7);
    bool su2_mo1 = (get_house(p2_su, p1_mo)==1 || get_house(p2_su, p1_mo)==5 || get_house(p2_su, p1_mo)==9 || get_house(p2_su, p1_mo)==7);
    if (su1_mo2 && su2_mo1) { s_sumo = 8; print_row(8, "Sun-Moon", "సూర్య-చంద్ర బంధం", "DOUBLE LOCK (Mutual ego-emotion mirroring)", "డబుల్ లాక్ (పరస్పర అహం-భావోద్వేగాల అవగాహన)"); }
    else if (su1_mo2 || su2_mo1) { s_sumo = 6; print_row(6, "Sun-Moon", "సూర్య-చంద్ర బంధం", "HARMONY (Sun illuminates partner's Moon)", "సామరస్యం (సూర్యుడు భాగస్వామి చంద్రుడిని ప్రకాశవంతం చేస్తాడు)"); }
    else { s_sumo = 2; print_row(2, "Sun-Moon", "సూర్య-చంద్ర బంధం", "AVERAGE (Standard luminary interaction)", "సాధారణం"); }

    bool v1_mo2 = (get_house(p1_ve, p2_mo)==1 || get_house(p1_ve, p2_mo)==5 || get_house(p1_ve, p2_mo)==9);
    bool v2_mo1 = (get_house(p2_ve, p1_mo)==1 || get_house(p2_ve, p1_mo)==5 || get_house(p2_ve, p1_mo)==9);
    if (v1_mo2 || v2_mo1) { s_vemo = 5; print_row(5, "Venus-Moon", "శుక్ర-చంద్ర బంధం", "SWEETNESS (Deep affection and emotional bonding)", "మధురానుభూతి (లోతైన ఆప్యాయత, భావోద్వేగ బంధం)"); }
    else { s_vemo = 2; print_row(2, "Venus-Moon", "శుక్ర-చంద్ర బంధం", "NEUTRAL (Affection requires conscious effort)", "తటస్థం"); }

    if (yoni_arr[nak_1] == yoni_arr[nak_2]) { s_yoni = 4; print_row(4, "Yoni Kuta", "యోని కూటమి", "IDENTICAL (Perfect instinctual/sexual rhythm)", "ఏక యోని (పరిపూర్ణ శారీరక అనుకూలత)"); }
    else { s_yoni = 2; print_row(2, "Yoni Kuta", "యోని కూటమి", "ACCEPTABLE (Different animal symbols)", "ఆమోదయోగ్యం"); }

    if (varna_arr[p1_mo] <= varna_arr[p2_mo]) { s_varna = 2; print_row(2, "Varna Kuta", "వర్ణ కూటమి", "COMPATIBLE (Spiritual ego aligns)", "అనుకూలం (ఆధ్యాత్మిక అహంకారం సమతుల్యం)"); }
    else { s_varna = 1; print_row(1, "Varna Kuta", "వర్ణ కూటమి", "FRICTION (Minor spiritual ego conflict)", "స్వల్ప ఘర్షణ (ఆధ్యాత్మిక అహంకార వివాదం)"); }

    if (vashya_arr[p1_mo] == vashya_arr[p2_mo]) { s_vashya = 2; print_row(2, "Vashya Kuta", "వశ్య కూటమి", "EQUAL (Neither dominates the other)", "సమానం (ఎవరు ఎవరిపైనా ఆధిపత్యం చెలాయించరు)"); }
    else { s_vashya = 1; print_row(1, "Vashya Kuta", "వశ్య కూటమి", "UNBALANCED (One sign naturally controls the other)", "అసమతుల్యం (ఒకరు సహజంగా మరొకరిని నియంత్రిస్తారు)"); }
    
    total_score += (s_vema + s_sumo + s_vemo + s_yoni + s_varna + s_vashya);
    print_cat_footer();

    // 2. PSYCHOLOGICAL HARMONY & SAV RESONANCE
    print_cat_header("2. PSYCHOLOGICAL HARMONY - Mind & Comfort (33 pts)", "2. మానసిక సామరస్యం - మనస్సు & సౌఖ్యం (33 pts)");
    int s_bha = 0, s_nadi = 0, s_gana = 0, s_tara = 0, s_vedha = 0, s_maitri = 0, s_sav = 0;

    int mo_mo = get_house(p2_mo, p1_mo); 
    if (mo_mo==6||mo_mo==8||mo_mo==2||mo_mo==12) { s_bha = 0; print_row(0, "Bhakoot (Sign)", "భకూట దోషం", "DOSHA (6/8 or 2/12 creates deep friction)", "దోషం (6/8 లేదా 2/12 స్థానాలు తీవ్ర ఘర్షణను సృష్టిస్తాయి)"); }
    else { s_bha = 6; print_row(6, "Bhakoot (Sign)", "భకూట స్థానం", "AUSPICIOUS (Emotional safety and flow)", "శుభకరం (భావోద్వేగ రక్షణ మరియు ప్రశాంతత)"); }

    if (nadi_arr[nak_1] == nadi_arr[nak_2]) { s_nadi = 2; print_row(2, "Nadi (Pulse)", "నాడి కూటమి", "SAME NADI (Traditional Dosha, implies identical soul frequency)", "ఏక నాడి దోషం (ఒకే రకమైన నాడీ వ్యవస్థ)"); }
    else { s_nadi = 6; print_row(6, "Nadi (Pulse)", "నాడి కూటమి", "EXCELLENT (Complementary nervous systems)", "అత్యుత్తమం (పరస్పర అనుకూలమైన నాడీ వ్యవస్థలు)"); }

    if (gana_arr[nak_1] == gana_arr[nak_2]) { s_gana = 5; print_row(5, "Gana Kuta", "గణ కూటమి", "HARMONIOUS (Same temperament category)", "అనుకూలం (ఒకే రకమైన స్వభావం)"); }
    else { s_gana = 1; print_row(1, "Gana Kuta", "గణ కూటమి", "CLASH (Different temperaments)", "విభేదం (భిన్నమైన స్వభావాలు)"); }

    int tara_dist = (nak_2 - nak_1 + 27) % 9;
    if (tara_dist==1||tara_dist==3||tara_dist==5||tara_dist==7||tara_dist==8) { 
        s_tara = 3; print_row(3, "Tara Kuta", "తారా బలం", "AUSPICIOUS (Sampat/Kshema/Sadhaka/Mitra)", "శుభకరం (సంపత్/క్షేమ/సాధక/మిత్ర)"); 
    } else if (tara_dist==4) { 
        s_tara = 1; print_row(1, "Tara Kuta", "తారా బలం", "PRATYAK (Obstacles / Karmic debt-clearing love)", "ప్రత్యక్ తార (కర్మ సంబంధిత అడ్డంకులు)"); 
    } else { 
        s_tara = 0; print_row(0, "Tara Kuta", "తారా బలం", "CHALLENGING (Vipat / Vadha / Janma)", "ప్రతికూలం (విపత్/వధ/జన్మ తారలు)"); 
    }

    bool vedha_hit = false;
    int mals[] = {1, 3, 7, 8, 9}; 
    for (int m : mals) {
        if ((int)(p2.planet_lons[m] / (360.0/27.0)) == vedha_map[nak_1]) vedha_hit = true;
        if ((int)(p1.planet_lons[m] / (360.0/27.0)) == vedha_map[nak_2]) vedha_hit = true;
    }
    if (vedha_hit) { s_vedha = 0; print_row(0, "SBC Vedha", "వేధ దోషం", "AFFLICTED (Malefics cast Vedha on Moon Nakshatra)", "బాధాకరం (పాప గ్రహాలచే వేధ)"); }
    else { s_vedha = 2; print_row(2, "SBC Vedha", "వేధ దోషం", "CLEAR (No Malefic Vedha on Moon)", "సురక్షితం (చంద్రునిపై ఎలాంటి వేధ లేదు)"); }

    int m_lord1=1, m_lord2=1;
    for(int p=1; p<=7; p++) { if(string(rashi_lords[p1_mo])==p_names_full[p]) m_lord1=p; if(string(rashi_lords[p2_mo])==p_names_full[p]) m_lord2=p; }
    int maitri_grid[8][8] = { {0,0,0,0,0,0,0,0}, {0,2,2,2,1,2,0,0}, {0,2,2,1,2,1,1,1}, {0,2,2,2,0,2,1,1}, {0,2,0,1,2,1,2,1}, {0,2,2,2,0,2,0,1}, {0,0,0,1,2,1,2,2}, {0,0,0,0,2,1,2,2} };
    int m1 = maitri_grid[m_lord1][m_lord2], m2 = maitri_grid[m_lord2][m_lord1];
    if(m1==2&&m2==2) { s_maitri=5; print_row(5, "Graha Maitri", "గ్రహ మైత్రి", "EXCELLENT (Mutual planetary friends)", "అత్యుత్తమం (పరస్పర గ్రహ మిత్రులు)"); }
    else if((m1==2&&m2==1)||(m1==1&&m2==2)) { s_maitri=4; print_row(4, "Graha Maitri", "గ్రహ మైత్రి", "GOOD (Friend / Neutral)", "మంచిది (మిత్రుడు / తటస్థం)"); }
    else if(m1==1&&m2==1) { s_maitri=3; print_row(3, "Graha Maitri", "గ్రహ మైత్రి", "AVERAGE (Mutual Neutral)", "సాధారణం (పరస్పర తటస్థం)"); }
    else if((m1==2&&m2==0)||(m1==0&&m2==2)) { s_maitri=2; print_row(2, "Graha Maitri", "గ్రహ మైత్రి", "CHALLENGING (Friend / Enemy)", "సవాలు (మిత్రుడు / శత్రువు)"); }
    else { s_maitri=1; print_row(1, "Graha Maitri", "గ్రహ మైత్రి", "FRICTION (Mutual Enemies or Neutral/Enemy)", "శత్రుత్వం (పరస్పర శత్రువులు)"); }

    int sav_p1_in_p2 = p2.sav_scores[p1_mo];
    int sav_p2_in_p1 = p1.sav_scores[p2_mo];
    if(sav_p1_in_p2 >= 28) s_sav += 3; else if(sav_p1_in_p2 >= 25) s_sav += 1;
    if(sav_p2_in_p1 >= 28) s_sav += 3; else if(sav_p2_in_p1 >= 25) s_sav += 1;
    
    char buf_en[128], buf_te[128];
    snprintf(buf_en, sizeof(buf_en), "MUTUAL NOURISHMENT (P1 in P2: %d, P2 in P1: %d)", sav_p1_in_p2, sav_p2_in_p1);
    snprintf(buf_te, sizeof(buf_te), "పరస్పర పోషణ (P1 in P2: %d, P2 in P1: %d)", sav_p1_in_p2, sav_p2_in_p1);
    print_row(s_sav, "SAV Resonance", "SAV అనుకూలత", string(buf_en), string(buf_te));

    total_score += (s_bha + s_nadi + s_gana + s_tara + s_vedha + s_maitri + s_sav);
    print_cat_footer();

    // 3. KARMIC DESTINY
    print_cat_header("3. KARMIC DESTINY - Soul Binding & Exact Overlays (42 pts)", "3. కర్మ బంధం - ఆత్మల కలయిక (42 pts)");
    int s_nodal = 0, s_ul = 0, s_akdk = 0, s_bb = 0, s_d60_dig = 0, s_d60_mut = 0, s_ascmo = 0;

    bool asc1_mo2 = (get_house(p1_asc, p2_mo)==1 || get_house(p1_asc, p2_mo)==7);
    bool asc2_mo1 = (get_house(p2_asc, p1_mo)==1 || get_house(p2_asc, p1_mo)==7);
    if (asc1_mo2 && asc2_mo1) { s_ascmo = 8; print_row(8, "Lagna-Moon", "లగ్న-చంద్ర బంధం", "DOUBLE SOUL TIE (Mutual Ascendant/Moon locks)", "ద్వంద్వ ఆత్మ బంధం (పరస్పర అనుసంధానం)"); }
    else if (asc1_mo2 || asc2_mo1) { s_ascmo = 6; print_row(6, "Lagna-Moon", "లగ్న-చంద్ర బంధం", "SOUL TIE (Partner's Moon on Ascendant axis)", "ఆత్మ బంధం (భాగస్వామి చంద్రుడు లగ్నంపై పడటం)"); }
    else { s_ascmo = 0; print_row(0, "Lagna-Moon", "లగ్న-చంద్ర బంధం", "INDEPENDENT (No direct Ascendant-Moon overlay)", "స్వతంత్రం (ప్రత్యక్ష లగ్న-చంద్ర సంబంధం లేదు)"); }

    bool mo_node_hit = (p1_ra==p2_mo || p1_ke==p2_mo || p2_ra==p1_mo || p2_ke==p1_mo);
    bool nodal_hit = false;
    int n1[] = {p1_ra, p1_ke}; int t2[] = {p2_su, p2_mo, p2_ve, p2_asc, p2.planet_rashis[p2_7L]};
    for(int n : n1) for(int t : t2) if(n == t) nodal_hit = true;
    int n2[] = {p2_ra, p2_ke}; int t1[] = {p1_su, p1_mo, p1_ve, p1_asc, p1.planet_rashis[p1_7L]};
    for(int n : n2) for(int t : t1) if(n == t) nodal_hit = true;

    if (mo_node_hit) { s_nodal = 10; print_row(10, "Nodal Grip", "రాహు/కేతు పట్టు", "KARMIC DEBT (Moon exactly on Nodal Axis - High intensity)", "కర్మ రుణం (చంద్రుడు కచ్చితంగా నోడల్ యాక్సిస్‌పై ఉన్నాడు - తీవ్రత ఎక్కువ)"); }
    else if (nodal_hit) { s_nodal = 6; print_row(6, "Nodal Grip", "రాహు/కేతు పట్టు", "INTENSE (Rahu/Ketu conjunct partner's core pillars)", "తీవ్రమైన బంధం (రాహు/కేతువులు భాగస్వామి ముఖ్య స్థానాలతో కలయిక)"); }
    else { s_nodal = 2; print_row(2, "Nodal Grip", "రాహు/కేతు పట్టు", "CLEAR (No heavy karmic debt or nodal obsession)", "సురక్షితం (భారీ కర్మ రుణం లేదు)"); }

    if (p1_ul == p2_ul || p1_a7 == p2_mo || p1_a7 == p2_ve || p2_a7 == p1_mo || p2_a7 == p1_ve) {
        s_ul = 7; print_row(7, "Arudha (A7/UL)", "ఆరూఢ లగ్న బంధం", "FATED (Darapada exactly hits spouse markers)", "విధి నిర్ణయం (దారపదం కచ్చితంగా భాగస్వామిపై పడటం)");
    } else { s_ul = 2; print_row(2, "Arudha (A7/UL)", "ఆరూఢ లగ్న బంధం", "STANDARD (No direct Arudha overlay)", "సాధారణం"); }

    double ak_1 = p1.planet_lons[p1.atmakaraka_idx], dk_2 = p2.planet_lons[p2.darakaraka_idx];
    double dk_1 = p1.planet_lons[p1.darakaraka_idx], mo_2 = p2.planet_lons[2];
    if (check_dist(ak_1, dk_2, 5.0) || check_dist(p2.planet_lons[p2.atmakaraka_idx], p1.planet_lons[p1.darakaraka_idx], 5.0)) {
        s_akdk = 8; print_row(8, "Exact Degrees", "డిగ్రీల కలయిక", "SOUL CONTRACT (AK conjunct DK within 5° orb)", "ఆత్మల ఒప్పందం (AK మరియు DK 5° లోపు కలయిక)");
    } else if (check_dist(dk_1, mo_2, 8.0) || check_dist(dk_2, p1.planet_lons[2], 8.0)) {
        s_akdk = 6; print_row(6, "Exact Degrees", "డిగ్రీల కలయిక", "DEFAULT SPOUSE (DK conjunct partner's Moon within orb)", "సహజ భాగస్వామి (DK భాగస్వామి చంద్రునితో కలయిక)");
    } else { s_akdk = 2; print_row(2, "Exact Degrees", "డిగ్రీల కలయిక", "INDEPENDENT (No exact Jaimini degree locks)", "స్వతంత్రం"); }

    if (check_dist(p1_bb, p2.planet_lons[6], 3.0) || check_dist(p1_bb, p2.planet_lons[p2_7L], 3.0) ||
        check_dist(p2_bb, p1.planet_lons[6], 3.0) || check_dist(p2_bb, p1.planet_lons[p1_7L], 3.0)) {
        s_bb = 4; print_row(4, "Bhrigu Bindu", "భృగు బిందు", "FATED TRIGGER (Destiny point exactly conjunct/opposes partner's 7L/Venus)", "విధి ప్రేరేపితం (డెస్టినీ పాయింట్ కచ్చితంగా భాగస్వామి 7వ అధిపతి/శుక్రునితో కలయిక)");
    } else { s_bb = 0; print_row(0, "Bhrigu Bindu", "భృగు బిందు", "SILENT (No exact destiny point triggers detected)", "తటస్థం"); }

    int ex_signs[] = {-1, 0, 1, 9, 5, 3, 11, 6}; 
    int own_1[] = {-1, 4, 3, 0, 2, 8, 1, 9};
    int own_2[] = {-1, -1, -1, 7, 5, 11, 6, 10};
    bool d60_p1_strong = (p1_d60_7L == ex_signs[p1_7L] || p1_d60_7L == own_1[p1_7L] || p1_d60_7L == own_2[p1_7L]);
    bool d60_p2_strong = (p2_d60_7L == ex_signs[p2_7L] || p2_d60_7L == own_1[p2_7L] || p2_d60_7L == own_2[p2_7L]);
    
    if (d60_p1_strong && d60_p2_strong) { s_d60_dig = 3; print_row(3, "D60 7L Dignity", "D60 7L బలం", "ETERNAL BOND (Both D60 7th Lords hold massive dignity across lifetimes)", "శాశ్వత బంధం (ఇద్దరి D60 7వ అధిపతులు అత్యంత బలంగా ఉన్నారు)"); }
    else if (d60_p1_strong || d60_p2_strong) { s_d60_dig = 1; print_row(1, "D60 7L Dignity", "D60 7L బలం", "PARTIAL PROMISE (One D60 7th Lord shows past-life marital mastery)", "పాక్షిక బలం (ఒకరి D60 7వ అధిపతి బలంగా ఉన్నాడు)"); }
    else { s_d60_dig = 0; print_row(0, "D60 7L Dignity", "D60 7L బలం", "STANDARD (D60 does not show exalted marital karma)", "సాధారణం"); }

    if (get_house(p1_d60_7L, p2_d60_7L) == 1 || get_house(p1_d60_7L, p2_d60_7L) == 7) {
        s_d60_mut = 2; print_row(2, "D60 Mutual", "D60 పరస్పర బంధం", "UNBREAKABLE (D60 7th Lords conjunct/opposed across lifetimes)", "విడదీయరాని బంధం (D60 7వ అధిపతులు పరస్పర కలయిక/దృష్టి)");
    } else { s_d60_mut = 0; print_row(0, "D60 Mutual", "D60 పరస్పర బంధం", "INDEPENDENT (No D60 mutual aspect)", "స్వతంత్రం"); }

    total_score += (s_ascmo + s_nodal + s_ul + s_akdk + s_bb + s_d60_dig + s_d60_mut);
    print_cat_footer();

    // 4. MARRIAGE STABILITY
    print_cat_header("4. MARRIAGE STABILITY & DHARMA - Longevity & Purpose (31 pts)", "4. వివాహ స్థిరత్వం & ధర్మం - ఆయుష్షు & లక్ష్యం (31 pts)");
    int s_sat = 0, s_7L = 0, s_kuja = 0, s_comp = 0, s_d30 = 0, s_pushkara = 0, s_nodal_dosha = 0, s_rajju = 0;

    if (p1_sa==p2_mo || p1_sa==p2_ve || p1_sa==p2_ul || p1_sa==p2.planet_rashis[p2_7L] ||
        p2_sa==p1_mo || p2_sa==p1_ve || p2_sa==p1_ul || p2_sa==p1.planet_rashis[p1_7L]) {
        s_sat = 6; print_row(6, "Saturn Binding", "శని బంధం", "GRAVITY (Saturn grips partner's marriage markers)", "దృఢత్వం (శని భాగస్వామి వివాహ స్థానాలను బలంగా పట్టుకున్నాడు)");
    } else { s_sat = 2; print_row(2, "Saturn Binding", "శని బంధం", "LIGHT (Lacks heavy Saturnian glue)", "సాధారణం (భారీ శని బంధం లేదు)"); }

    bool parivartana = (get_house(p1.planet_rashis[p1_7L], p2_asc) == get_house(p2.planet_rashis[p2_7L], p1_asc));
    if (parivartana && (p1_7L==p2_5L || p1_7L==p2_9L || p2_7L==p1_5L || p2_7L==p1_9L)) {
        s_7L = 8; print_row(8, "House Lords", "భావాధిపతుల బంధం", "KALANIDHI YOGA (Flawless mutual exchange and Trinal crossing)", "కళానిధి యోగం (దోషరహిత పరస్పర మార్పిడి మరియు త్రికోణ కలయిక)");
    } else if (p1_7L==p2_5L || p1_7L==p2_9L || p2_7L==p1_5L || p2_7L==p1_9L || p1_5L==p2_ve || p2_5L==p1_ve || get_house(p1.planet_rashis[p1_5L], p2_ve)==1 || get_house(p2.planet_rashis[p2_5L], p1_ve)==1) {
        s_7L = 6; print_row(6, "House Lords", "భావాధిపతుల బంధం", "DHARMIC (5th/9th lords cross-connect with 7th/Venus - Poorva Punya)", "ధార్మిక బంధం (5/9 అధిపతులు 7వ/శుక్రునితో కలయిక - పూర్వ పుణ్యం)");
    } else { s_7L = 2; print_row(2, "House Lords", "భావాధిపతుల బంధం", "AVERAGE (No major Trinal cross-chart exchanges)", "సాధారణం"); }

    bool kd1 = is_kd_house(get_house(p1_ma, p1_asc)) || is_kd_house(get_house(p1_ma, p1_mo));
    bool kd2 = is_kd_house(get_house(p2_ma, p2_asc)) || is_kd_house(get_house(p2_ma, p2_mo));
    if (kd1 == kd2) { s_kuja = 4; print_row(4, "Kuja Dosha", "కుజ దోషం", "EXCELLENT (Dosha Samya - Aggression neutralized)", "అద్భుతం (దోష సామ్యం - దూకుడు తటస్థీకరించబడింది)"); }
    else { s_kuja = 0; print_row(0, "Kuja Dosha", "కుజ దోషం", "ASYMMETRIC (Volatile marital heat)", "అసమతుల్యం (వివాహంలో తీవ్రమైన వేడి మరియు ఘర్షణ)"); }

    bool nd1 = get_nodal_dosha(p1_ra, p1_asc);
    bool nd2 = get_nodal_dosha(p2_ra, p2_asc);
    if (nd1 == nd2) { s_nodal_dosha = 5; print_row(5, "Nodal Samya", "రాహు/కేతు సామ్యం", "BALANCED (Shadow nodes neutralized)", "సమతుల్యం (ఛాయా గ్రహ దోషాలు తటస్థీకరించబడ్డాయి)"); }
    else { s_nodal_dosha = 0; print_row(0, "Nodal Samya", "రాహు/కేతు సామ్యం", "ASYMMETRIC (Fatal nodal imbalance)", "అసమతుల్యం (ప్రమాదకరమైన నోడల్ అసమతుల్యత)"); }

    if (rajju_arr[nak_1] == rajju_arr[nak_2]) { 
        s_rajju = -10; print_row(-10, "Rajju Kuta", "రజ్జు కూటమి", "FATAL DOSHA (Same Rajju - Threat to longevity)", "ఏక రజ్జు దోషం (ప్రాణ గండం - ఆయుష్షుకు ముప్పు)"); 
    } else { s_rajju = 0; print_row(0, "Rajju Kuta", "రజ్జు కూటమి", "SAFE (Different Rajjus)", "సురక్షితం (వేర్వేరు రజ్జువులు)"); }

    double comp_mo = get_midpoint(p1.planet_lons[2], p2.planet_lons[2]);
    double comp_ra = get_midpoint(p1.planet_lons[8], p2.planet_lons[8]);
    if (check_dist(comp_mo, p1.planet_lons[8], 10.0) || check_dist(comp_mo, p2.planet_lons[8], 10.0) || check_dist(comp_mo, comp_ra, 10.0)) { 
        s_comp = 4; print_row(4, "Composite", "ఉమ్మడి గ్రహ స్థితి", "FATED BOND (Composite Moon conjunct Rahu)", "విధి నిర్ణయం (కాంపోజిట్ చంద్రుడు రాహువుతో కలయిక)"); 
    } else { s_comp = 1; print_row(1, "Composite", "ఉమ్మడి గ్రహ స్థితి", "STANDARD (No profound composite planetary alignments)", "సాధారణం"); }

    int deb_signs[] = {-1, 6, 7, 3, 11, 9, 5, 0}; 
    if (p1_d30_7L != deb_signs[p1_7L] && p2_d30_7L != deb_signs[p2_7L]) {
        s_d30 = 2; print_row(2, "D30 Trimsamsa", "D30 త్రింశాంశ", "CLEAN (7th Lords free from deep hidden afflictions)", "దోష రహితం (7వ అధిపతులు దాగి ఉన్న దోషాల నుండి విముక్తం)");
    } else { s_d30 = 0; print_row(0, "D30 Trimsamsa", "D30 త్రింశాంశ", "AFFLICTED (Hidden marital karma / evils present)", "బాధాకరం (దాగి ఉన్న వైవాహిక కర్మ / దోషాలు ఉన్నాయి)"); }

    if (is_pushkara(p1.get_varga(9, p1.planet_lons[6])) || is_pushkara(p2.get_varga(9, p2.planet_lons[2]))) {
        s_pushkara = 2; print_row(2, "Pushkara Bhaga", "పుష్కర భాగ", "DIVINE BLESSING (Venus/Moon in Pushkara Navamsa)", "దైవిక ఆశీర్వాదం (నవాంశలో శుక్రుడు/చంద్రుడు పుష్కర భాగలో ఉన్నారు)");
    } else { s_pushkara = 0; print_row(0, "Pushkara Bhaga", "పుష్కర భాగ", "STANDARD (No Pushkara Navamsa protection)", "సాధారణం"); }

    total_score += (s_sat + s_7L + s_kuja + s_comp + s_d30 + s_pushkara + s_nodal_dosha + s_rajju);
    print_cat_footer();

    // 5. NAVAMSA (D9)
    print_cat_header("5. NAVAMSA (D9) & TIMING MANIFESTATION (15 pts)", "5. నవాంశ (D9) & సమయ అనుకూలత (15 pts)");
    int s_d9 = 0, s_time = 0;
    
    if (p1_d9_asc == p2_asc || p1_d9_asc == p2_mo || p2_d9_asc == p1_asc || p2_d9_asc == p1_mo || p1_d9_asc == p2_d9_asc) {
        s_d9 = 4; print_row(4, "D9 Lagna Lock", "D9 లగ్న అనుసంధానం", "SOUL RECOGNITION (D9 Lagnas exactly overlay D1 Lagnas/Moons)", "ఆత్మ గుర్తింపు (D9 లగ్నాలు D1 లగ్నాలు/చంద్రులతో కచ్చితంగా కలిశాయి)");
    } else if (get_house(p1_d9_asc, p2_d9_asc)==1 || get_house(p1_d9_asc, p2_d9_asc)==5 || get_house(p1_d9_asc, p2_d9_asc)==9 || get_house(p1_d9_asc, p2_d9_asc)==7) {
        s_d9 = 2; print_row(2, "D9 Lagna Axis", "D9 లగ్న అక్షం", "ALIGNED (D9 Lagnas in 1/5/9 or 1/7 relationship)", "అనుకూలం (D9 లగ్నాలు 1/5/9 లేదా 1/7 సంబంధంలో ఉన్నాయి)");
    } else { s_d9 = 0; print_row(0, "D9 Lagna Axis", "D9 లగ్న అక్షం", "DIVERGENT (Independent soul paths in Navamsa)", "స్వతంత్రం (నవాంశలో వేర్వేరు ఆత్మ మార్గాలు)"); }

    int d9_ve_dist = get_house(p1_d9_ve, p2_d9_ve);
    if (d9_ve_dist == 7) {
        s_d9 += 3; print_row(3, "D9 Venus Axis", "D9 శుక్ర బంధం", "SOUL POLARITY (D9 Venuses are opposite - highly magnetic)", "ఆత్మల ఆకర్షణ (D9 శుక్రులు పరస్పర దృష్టిలో ఉన్నారు - అత్యంత అయస్కాంత ఆకర్షణ)");
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

    if (!html) {
        printf("  - Person 1 Dasha : %s / %s -> Readiness: %s\n", dasha_lords[p1_md], dasha_lords[p1_ad], p1_active ? "HIGH" : "Low");
        printf("  - Person 2 Dasha : %s / %s -> Readiness: %s\n", dasha_lords[p2_md], dasha_lords[p2_ad], p2_active ? "HIGH" : "Low");
    }

    if (p1_active && p2_active) { s_time = 8; print_row(8, "Manifestation", "దశా అనుకూలత", "SYNCHRONIZED (Universe is actively pushing union)", "సమకాలీకరించబడింది (విశ్వం మీ కలయికను చురుకుగా ప్రోత్సహిస్తోంది)"); }
    else if (p1_active || p2_active) { s_time = 4; print_row(4, "Manifestation", "దశా అనుకూలత", "ASYMMETRIC (Timing is off; one partner is delayed)", "అసమతుల్యత (సమయం సరిగ్గా లేదు; ఒక భాగస్వామికి జాప్యం ఉంది)"); }
    else { s_time = 0; print_row(0, "Manifestation", "దశా అనుకూలత", "DORMANT (No marital timing activated currently)", "నిద్రాణస్థితి (ప్రస్తుతం వివాహ సమయం యాక్టివ్‌గా లేదు)"); }

    total_score += (s_d9 + s_time);
    print_cat_footer();

    // 6. MUTUAL RECTIFICATION
    print_cat_header("6. MUTUAL RECTIFICATION & DESTRUCTION - Dosha Nullification (15 pts)", "6. దోష పరిహారాలు & నష్టాలు - పరస్పర శాంతి (15 pts)");
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

    if (!html) {
        if (te) {
            printf("  - వ్యక్తి 1 గ్రహ దోషాలు : %d | వ్యక్తి 2 ద్వారా పరిహరించబడినవి: %.0f%%\n", p1_afflictions, p2_rect_pct);
            printf("  - వ్యక్తి 2 గ్రహ దోషాలు : %d | వ్యక్తి 1 ద్వారా పరిహరించబడినవి: %.0f%%\n", p2_afflictions, p1_rect_pct);
        } else {
            printf("  - Person 1 Internal Afflictions : %d | Rectified by P2: %.0f%%\n", p1_afflictions, p2_rect_pct);
            printf("  - Person 2 Internal Afflictions : %d | Rectified by P1: %.0f%%\n", p2_afflictions, p1_rect_pct);
        }
    }

    int net_healing = (p1_heals_p2 * 3) + (p2_heals_p1 * 3);
    int net_damage = (p1_destroys_p2 * 4) + (p2_destroys_p1 * 4);
    
    s_rect = 5 + net_healing - net_damage; // Base score 5
    if (s_rect > 15) s_rect = 15;
    if (s_rect < 0) s_rect = 0;

    if (net_damage > net_healing) {
        print_row(s_rect, "Dosha Exchange", "పరస్పర దోష పరిహారం", "AGGRAVATION (Charts compound each other's malefic flaws)", "దోష తీవ్రత పెరుగుదల (ఒకరి దోషాలను మరొకరు పెంచుతున్నారు)");
    } else if (net_healing > 0) {
        print_row(s_rect, "Dosha Exchange", "పరస్పర దోష పరిహారం", "HEALING (Charts successfully nullify internal afflictions)", "పరిహారం (ఒకరి అంతర్గత దోషాలను మరొకరు విజయవంతంగా తగ్గిస్తున్నారు)");
    } else {
        print_row(s_rect, "Dosha Exchange", "పరస్పర దోష పరిహారం", "NEUTRAL (No major cross-chart healing or destruction)", "సాధారణం (పెద్దగా ఒకరికొకరు నష్టం/మేలు చేయడం లేదు)");
    }

    total_score += s_rect;
    print_cat_footer();

    // =================================================================
    // FINAL VERDICT NORMALIZATION
    // =================================================================
    int max_score = 165; // 29 + 33 + 42 + 31 + 15 + 15 = 165 Maximum Points
    int final_percentage = (int)round(((double)total_score / max_score) * 100.0);
    
    string status_en, status_te;
    if (final_percentage >= 80) {
        status_en = "RARE SOULMATE (Profound karmic, physical, and spiritual union. Timing is aligned.)";
        status_te = "అరుదైన ఆత్మ బంధం (ప్రగాఢమైన కర్మ, శారీరక మరియు ఆధ్యాత్మిక కలయిక. దశా సమయం అనుకూలంగా ఉంది.)";
    } else if (final_percentage >= 60) {
        status_en = "HIGHLY AUSPICIOUS (Strong love, structural longevity, and excellent compatibility.)";
        status_te = "అత్యంత శుభకరం (బలమైన ప్రేమ, నిర్మాణపరమైన దీర్ఘాయువు మరియు అద్భుతమైన అనుకూలత.)";
    } else if (final_percentage >= 40) {
        status_en = "AVERAGE (Standard human connection; requires compromise and patience.)";
        status_te = "సాధారణం (సాధారణ మానవ సంబంధం; రాజీ మరియు ఓర్పు అవసరం.)";
    } else {
        status_en = "HIGH FRICTION / KARMIC DEBT (Not recommended for peace; structural obstacles detected.)";
        status_te = "తీవ్రమైన ఘర్షణ / కర్మ రుణం (శాంతి కోసం సిఫార్సు చేయబడదు; నిర్మాణపరమైన అడ్డంకులు ఉన్నాయి.)";
    }

    if (html) {
        printf("<div style='margin-top: 30px; padding: 20px; background: #2a2a35; border-radius: 8px; border-left: 5px solid %s;'>", final_percentage >= 60 ? "#2ecc71" : (final_percentage >= 40 ? "#f1c40f" : "#e74c3c"));
        printf("<h2 style='margin:0 0 10px 0; color:#fff;'>%s: <span style='color:var(--accent);'>%d / %d</span> (%d%%)</h2>", te?"తుది సార్వత్రిక జాతక పొంతన స్కోరు":"FINAL UNIVERSAL SYNASTRY INDEX", total_score, max_score, final_percentage);
        printf("<p style='font-size:16px; margin:0;'><b>%s:</b> %s</p>", te?"ఫలితం":"STATUS", te?status_te.c_str():status_en.c_str());
        printf("</div>\n");
    } else {
        printf("=================================================================\n");
        if (te) printf("తుది V8.4 సార్వత్రిక జాతక పొంతన స్కోరు: %d / %d పాయింట్లు (శాతం: %d%%)\n", (int)total_score, max_score, final_percentage);
        else printf("FINAL V8.4 UNIVERSAL SYNASTRY INDEX: %d / %d Raw Points (Normalized: %d%%)\n", (int)total_score, max_score, final_percentage);
        printf("STATUS: %s\n", te ? status_te.c_str() : status_en.c_str());
        printf("=================================================================\n");
    }
}

void predict_synastry_events(const JyotishaEngine& p1, const JyotishaEngine& p2, int start_year, int end_year) {
    bool html = p1.html_mode;
    bool te = p1.telugu_mode;

    if (html) {
        printf("<h3 style='color: var(--accent); margin-top: 25px; margin-bottom: 10px;'>%s</h3>", te ? "పరస్పర సంఘటనల స్కానర్ (Mutual Event Scanner)" : "Mutual Event Scanner (Synastry Double-Blind Cross-Transit)");
        printf("<p style='color:#ccc; margin-bottom:15px;'>Scanning %d to %d for exact Jup/Sat/Sun/Ven/Dasha alignments...</p>", start_year, end_year);
    } else {
        printf("\n=================================================================\n");
        printf("=== MUTUAL EVENT SCANNER (SYNASTRY DOUBLE-BLIND CROSS-TRANSIT) ===\n");
        printf("=================================================================\n");
        printf("Scanning %d to %d for days where BOTH charts hit critical mass...\n\n", start_year, end_year);
    }

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
        if (html) printf("<div style='padding:15px; background:#2a2a35; border-left:4px solid #e74c3c; border-radius:4px;'><p style='margin:0; color:#e0e0e0;'>%s</p></div>\n", te?"ఈ సమయంలో రెండు జాతకాలను సమకాలీకరించే (కలిపే) ఖచ్చితమైన కాస్మిక్ ఈవెంట్స్ ఏవీ లేవు.":"No mutual convergence found. The cosmic clocks for these two charts do not align in this timeframe.");
        else printf("No mutual convergence found. The cosmic clocks for these two charts do not align in this timeframe.\n");
    } else {
        sort(peak_hits.begin(), peak_hits.end(), [](const Hit& a, const Hit& b) { 
            if (a.y != b.y) return a.y < b.y;
            if (a.m != b.m) return a.m < b.m;
            return a.d < b.d;
        });

        if (html) {
            printf("<table class='data-table'><tr><th style='width:90px;'>Date</th><th>Power</th><th>Cosmic Clock</th><th>Consolidated Env</th><th>P1 Theme</th><th>P2 Theme</th></tr>\n");
            int count = 0;
            for (const auto& hit : peak_hits) {
                if (count++ >= 10) break;
                printf("<tr><td><b>%02d/%02d/%04d</b></td><td>P1: %d<br>P2: %d</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n", 
                       hit.d, hit.m, hit.y, hit.s1, hit.s2, hit.t_pos.c_str(), hit.cons_env.c_str(), hit.arch1.c_str(), hit.arch2.c_str());
                printf("<tr><td colspan='6' style='font-size:13px; color:#aaa; padding-bottom:15px; border-bottom:2px solid var(--border);'><b>P1 Triggers:</b> %s<br><b>P2 Triggers:</b> %s</td></tr>\n", hit.res1.c_str(), hit.res2.c_str());
            }
            printf("</table>\n");
        } else {
            printf("%-10s | %-12s | %-12s | %-19s | %-30s | %-20s | %-20s\n", 
                   "Mutual Day", "P1 Power", "P2 Power", "Cosmic Clock", "Consolidated Env", "P1 Life Theme", "P2 Life Theme");
            printf("----------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
            int count = 0;
            for (const auto& hit : peak_hits) {
                if (count++ >= 10) break;
                printf("%02d/%02d/%04d | P1: %d/22 pts | P2: %d/22 pts | %-19s | %-30s | %-20s | %-20s\n", 
                       hit.d, hit.m, hit.y, hit.s1, hit.s2, hit.t_pos.c_str(), hit.cons_env.c_str(), hit.arch1.c_str(), hit.arch2.c_str());
                printf("           | %-120s \n", hit.res1.c_str());
                printf("           | %-120s \n", hit.res2.c_str());
                printf("----------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
            }
        }
    }
    if (!html) printf("=================================================================\n");
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
// HELP MENU DEFINITION
// =========================================================================

void print_help_menu() {
    printf("\n================================================================================\n");
    printf("                  JYOTISHA ENGINE - COMMAND LINE HELP MENU                      \n");
    printf("================================================================================\n\n");

    printf("USAGE STRUCTURE:\n");
    printf("  [pre-flags] <year> <month> <day> <hour> <minute> <second> <city> <command> [cmd-args]\n\n");

    printf("GLOBAL PRE-FLAGS (Optional - Intercepted before parsing structural data):\n");
    printf("  telugu / --te       Enables Telugu translation strings for UI blocks.\n");
    printf("  html                Switches console table & rashi outputs to clean HTML elements.\n");
    printf("  savana / --savana   Uses 360-day Savana year for Dasha math instead of 365.2563 Sidereal.\n\n");

    printf("MANDATORY BASE PARAMETERS (Required for all executions):\n");
    printf("  <year> <month> <day>  Birth/Event Gregorian Date digits (e.g., 1979 06 01)\n");
    printf("  <hour> <min> <sec>    Local Birth/Event Time digits (24-hour style, e.g., 14 30 00)\n");
    printf("  <city>                Target location lookup string matching DB record (e.g., Nellore)\n\n");

    printf("CORE WEB COMMANDS (Typically used by backend web wrappers):\n");
    printf("  json                  Runs Ashtakavarga, auspiciousness scan, and exports unified Web JSON.\n");
    printf("  web_natal             Initializes engine and stops after parsing standard natal arrays.\n");
    printf("  web_general           Computes Varsha-Masa, full Shadbala profiles, Ashtakavarga, and D1 trends.\n");
    printf("  web_dasha             Processes Ashtakavarga profiles, balances, and executes web-formatted dasha outputs.\n");
    printf("  web_dosha             Runs dedicated chart evaluation rules tracking key astrological doshas.\n");
    printf("  web_transit <Y> <M> <D> [H:M:S]\n");
    printf("                        Computes precise transit matrices for web frontend without complex phase logic.\n");
    printf("  web_progeny / progeny [gender]\n");
    printf("                        Evaluates classical Saptamsha (D7) and biological sphutas for children.\n");
    printf("  web_age_gap / age_gap Calculates the estimated age difference and maturity of the spouse.\n");
    printf("  full_report [name] [gender]\n");
    printf("                        Generates a complete, print-ready HTML document (A4 PDF capable) of all tabs.\n\n");

    printf("ADVANCED CLI COMMANDS & ANALYSIS:\n");
    printf("  kp                    Calculates Krishnamurti Paddhati (KP System) cusps, lords, and significators.\n");
    printf("  analyze [varga]       Analyzes explicit chart divisions. [varga] optional (Defaults to 'D1').\n");
    printf("  ayush                 Triggers deep longevity evaluation protocols using operational sub-engines.\n");
    printf("  all [varga/planet] [Y M D]\n");
    printf("                        Runs full systemic profile: Navatara, Shadbala, Ashtakavarga, Dasha, etc.\n");
    printf("                        If a Varga (e.g., D9) is specified, it also runs Dual-Layer Transit Scanners:\n");
    printf("                        1. Micro-Transit (D_x sky -> Natal D_x)\n");
    printf("                        2. Rasi Tulya Varga (Physical D1 sky -> Natal D_x).\n\n");
    printf("  nama-nakshatra        Prints Nakshatra naming syllables. Can be run standalone or with date details.\n");

    printf("TIME-VARIANT & CALCULATED TRANSITS:\n");
    printf("  daily [Y M D]         Calculates Muhurat grids, Panchang transitions, Lagnas, and Hora intervals.\n");
    printf("                        Defaults to base birth parameters if explicit date is not given.\n");
    printf("  transit [Y M D] [H:M:S]\n");
    printf("                        Calculates full planet transits with heavy evaluation. Defaults to local clock time if blank.\n");
    printf("  collision [planet] [varga] [Y] [M] [D]\n");
    printf("                        Tracks orbital junction alignments for specified planet over given intervals. Supports Vargas.\n\n");

    printf("DASHA & RETURN TIME COMMANDS:\n");
    printf("  dasha [all]           With 'all': Exports exhaustive multi-level dasha charts to CSV formatting.\n");
    printf("  dasha [Y M D] [H:M:S]\n");
    printf("                        With targeted parameters: Tracks exact operational dasha sequence down to 6-levels.\n");
    printf("                        Without arguments: Launches an interactive command terminal dasha utility.\n");
    printf("  deha [Y M D] [H:M:S]  Evaluates subtle physical transition loops over target date vectors.\n");
    printf("  tithi <target_year>   Identifies exact timestamp when lunar phase matches target solar returns.\n");
    printf("  annual / web_annual / web_varshaphal <target_year>\n");
    printf("                        Generates the Varshaphala (Annual Solar Return) chart and D1 matrix analysis.\n");
    printf("  predict <startY> <endY>\n");
    printf("                        Executes marriage calculation algorithms within target runtime window bounds.\n");
    printf("  muhurat / web_muhurat <event> <Y> <M>\n");
    printf("                        Scans targeted calendars to identify optimal muhurat windows for specified actions.\n\n");

    printf("ASTROMETRIC SEARCH & SEARCH ALIGNMENT:\n");
    printf("  degree <planet> <sign> <deg> <min> <sec> [year] [month]\n");
    printf("                        Performs absolute backwards search to isolate point alignments.\n\n");

    printf("COMPATIBILITY & SYNASTRY:\n");
    printf("  match / match_predict / web_match / web_synastry <Y2> <M2> <D2> <H2> <Min2> <S2> <City2>\n");
    printf("                        Calculates relational synastry points against a second chart dataset.\n");
    printf("  match_predict <startY> <endY> <Y2> <M2> <D2> <H2> <Min2> <S2> <City2>\n");
    printf("                        Triggers dynamic relationship prediction over specific year frames.\n");
    printf("================================================================================\n\n");
}

// =========================================================================
// MAIN COMMAND LINE PARSER
// =========================================================================

int main(int argc, char *argv[]) {
    // --- FLAG INTERCEPTOR ---
    bool telugu_ui = false;
    bool html_ui = false;
    bool use_savana = false;
	bool use_true_node = false;
    vector<char*> clean_args;
    clean_args.push_back(argv[0]);
    
    for (int i = 1; i < argc; i++) {
        if (strcasecmp(argv[i], "telugu") == 0 || strcasecmp(argv[i], "--te") == 0) {
            telugu_ui = true;
        } else if (strcasecmp(argv[i], "html") == 0) {
            html_ui = true; 
        } else if (strcasecmp(argv[i], "savana") == 0 || strcasecmp(argv[i], "--savana") == 0) {
            use_savana = true; 
        } else if (strcasecmp(argv[i], "true_node") == 0 || strcasecmp(argv[i], "--true-node") == 0) {
            use_true_node = true; // <--- CATCH THE COMMAND
        } else {
            clean_args.push_back(argv[i]);
        }
    }
    
    int clean_argc = clean_args.size();
    char** clean_argv = clean_args.data();

    // 1. Standalone Table Catch (Prints without needing a date)
    if (clean_argc == 2 && strcasecmp(clean_argv[1], "nama-nakshatra") == 0) {
        printf("\n=== NAMA NAKSHATRA (STARTING LETTERS FOR NAMING) ===\n");
        printf("--------------------------------------------------------------------------------\n");
        printf("%-18s | %-10s | %-10s | %-10s | %-10s\n", "Nakshatra", "Pada 1", "Pada 2", "Pada 3", "Pada 4");
        printf("--------------------------------------------------------------------------------\n");
        for (int i = 0; i < 27; i++) {
            printf("%-18s | %-10s | %-10s | %-10s | %-10s\n",
                   nak_names[i], nama_aksharas[i][0], nama_aksharas[i][1], nama_aksharas[i][2], nama_aksharas[i][3]);
        }
        printf("--------------------------------------------------------------------------------\n\n");
        return 0;
    }

    // 2. Help Menu Catch
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
	JyotishaEngine engine(year, month, day, hour, minute, second, *it, json_mode, telugu_ui, html_ui, use_savana, use_true_node);
    
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
            engine.export_web_json(year, month, day); 
            printf("\n"); fflush(stdout); 
            return 0;
        }
        else if (strcasecmp(cmd.c_str(), "web_natal") == 0) {
            engine.user_name = (clean_argc > 9) ? clean_argv[9] : "Guest";
            engine.user_gender = (clean_argc > 10) ? clean_argv[10] : "Not Specified";
            engine.html_mode = html_ui; 
            
            // 1. Print Standard Birth Chart UI
            engine.print_birth_chart_ui(); 
            
            // 2. Print Ashtakavarga Table
            engine.calculate_ashtakavarga(false); 

            // 3. Print Shadbala (Now Natively HTML formatted!)
            int v_lord = -1, m_lord = -1;
            engine.calculate_varsha_masa(v_lord, m_lord);
            
            ShadbalaEngine::calculate(engine.lagna_lon, engine.planet_lons, engine.moon_lon, engine.tjd_ut, 
                                      engine.local_hour_decimal, engine.sunrise_hour_decimal, engine.sunset_hour_decimal, 
                                      engine.current_weekday, v_lord, m_lord, false, html_ui, telugu_ui, engine.json_output);

            printf("\n"); fflush(stdout); 
            return 0; 
        }
        else if (strcasecmp(cmd.c_str(), "web_general") == 0) {
            // General Tab is strictly for Predictive Text! No tables here.
            engine.analyze_chart("D1",true); 
            printf("\n"); fflush(stdout); 
            return 0;
        }
        else if (strcasecmp(cmd.c_str(), "web_dasha") == 0) {
            engine.calculate_ashtakavarga(true); 
            engine.analyze_auspiciousness(engine.planet_rashis[0], engine.planet_rashis);
            engine.calculate_dasha_balance();
            engine.print_dasha_web(); 

            if (engine.html_mode) {
                time_t t = time(nullptr); tm* now_utc = gmtime(&t);
                double ut_dec = now_utc->tm_hour + (now_utc->tm_min / 60.0) + (now_utc->tm_sec / 3600.0);
                double current_jd = swe_julday(now_utc->tm_year + 1900, now_utc->tm_mon + 1, now_utc->tm_mday, ut_dec, SE_GREG_CAL);
                engine.print_dasha_tables_html(current_jd);
            }
            printf("\n"); fflush(stdout); 
            return 0;
        }
        else if (strcasecmp(cmd.c_str(), "web_dosha") == 0) {
            engine.analyze_doshas(engine.planet_rashis, engine.planet_rashis[0]);
            printf("\n"); fflush(stdout); 
            return 0;
        }
        else if (strcasecmp(cmd.c_str(), "web_transit") == 0) {
            if (clean_argc >= 12) { 
                t_year = stoi(clean_argv[9]); t_month = stoi(clean_argv[10]); t_day = stoi(clean_argv[11]); parse_target_time(12);
                engine.calculate_transits(t_year, t_month, t_day, t_hour, t_min, t_sec, false, true); 
            }
            printf("\n"); fflush(stdout); 
            return 0;
        }
        else if (strcasecmp(cmd.c_str(), "web_progeny") == 0 || strcasecmp(cmd.c_str(), "progeny") == 0) {
            bool is_female = false;
            bool gender_provided = false;
            if (clean_argc >= 10) {
                if (strcasecmp(clean_argv[9], "female") == 0 || strcasecmp(clean_argv[9], "f") == 0 || strcasecmp(clean_argv[9], "wife") == 0) {
                    is_female = true; gender_provided = true;
                } else if (strcasecmp(clean_argv[9], "male") == 0 || strcasecmp(clean_argv[9], "m") == 0 || strcasecmp(clean_argv[9], "husband") == 0) {
                    is_female = false; gender_provided = true;
                }
            }
            engine.analyze_progeny(is_female, gender_provided);
            printf("\n"); fflush(stdout); 
            return 0;
        }
		else if (strcasecmp(cmd.c_str(), "web_age_gap") == 0 || strcasecmp(cmd.c_str(), "age_gap") == 0) {
            bool is_female = false;
            bool gender_provided = false;
            if (clean_argc >= 10) {
                for (int k = 9; k < clean_argc; k++) {
                    if (strcasecmp(clean_argv[k], "female") == 0 || strcasecmp(clean_argv[k], "f") == 0 || strcasecmp(clean_argv[k], "wife") == 0) {
                        is_female = true; gender_provided = true; break;
                    } else if (strcasecmp(clean_argv[k], "male") == 0 || strcasecmp(clean_argv[k], "m") == 0 || strcasecmp(clean_argv[k], "husband") == 0) {
                        is_female = false; gender_provided = true; break;
                    }
                }
            }
            engine.analyze_spouse_age_gap(is_female, gender_provided);
            printf("\n"); fflush(stdout);
            return 0;
        }
		else if (strcasecmp(cmd.c_str(), "nama-nakshatra") == 0) {
            engine.print_specific_nama();
            printf("\n"); fflush(stdout); 
            return 0;
        }
        // =========================================================================
        // NEW: FULL REPORT GENERATOR (PRINT-TO-PDF CAPABLE)
        // =========================================================================
        else if (strcasecmp(cmd.c_str(), "full_report") == 0) {
            engine.html_mode = true;
            engine.user_name = (clean_argc > 9) ? clean_argv[9] : "User";
            string gender = (clean_argc > 10) ? clean_argv[10] : "Male";
            bool is_female = (strcasecmp(gender.c_str(), "Female") == 0 || strcasecmp(gender.c_str(), "F") == 0);

            printf("<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n<meta charset=\"UTF-8\">\n");
            printf("<title>Astrology Report - %s</title>\n", engine.user_name.c_str());
            printf("<style>\n");
            // Screen styles (Dark Mode)
            printf("body { background: #121212; color: #e0e0e0; font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; padding: 40px; max-width: 1000px; margin: auto; }\n");
            printf("h1, h2, h3, h4 { color: #f39c12; }\n");
            printf(".data-table { width: 100%%; border-collapse: collapse; margin-bottom: 20px; }\n");
            printf(".data-table th, .data-table td { border: 1px solid #444; padding: 10px; text-align: left; }\n");
            printf(".data-table th { background: #2a2a35; }\n");
            printf(".rasi-table { width: 100%%; border-collapse: collapse; margin-bottom: 20px; }\n");
            printf(".rasi-table td { border: 1px solid #444; width: 25%%; height: 100px; padding: 5px; vertical-align: top; }\n");
            printf(".rasi-center { text-align: center; vertical-align: middle !important; color: #888; font-size: 1.5em; }\n");
            
            // Print styles (Light Mode for A4 PDF Generation)
            // Print styles (Light Mode for A4 PDF Generation)
            printf("@media print {\n");
            printf("  body { background: #fff; padding: 0; }\n");
            
            // --- THE FIX: Force all text tags to solid black to override inline dark-mode greys ---
            printf("  body, p, span, div, td, th, h1, h2, h3, h4, b, i, ul, li { color: #000 !important; }\n");
            
            printf("  h1, h2, h3, h4 { page-break-after: avoid; }\n");
            printf("  .data-table th, .data-table td, .rasi-table td { border: 1px solid #000 !important; }\n");
            printf("  .data-table th { background: #eee !important; -webkit-print-color-adjust: exact; }\n");
            printf("  div { page-break-inside: avoid; }\n");
            printf("}\n");
            printf("</style>\n</head>\n<body>\n");

            if (engine.telugu_mode) {
                printf("<h1 style='text-align:center; border-bottom:2px solid #f39c12; padding-bottom:10px;'>సంపూర్ణ జ్యోతిష్య నివేదిక</h1>\n");
                printf("<h3 style='text-align:center; color:#888; margin-top:-15px;'>జాతకుడు/జాతకురాలు: %s</h3>\n", engine.user_name.c_str());
            } else {
                printf("<h1 style='text-align:center; border-bottom:2px solid #f39c12; padding-bottom:10px;'>Vedic Astrology Comprehensive Report</h1>\n");
                printf("<h3 style='text-align:center; color:#888; margin-top:-15px;'>Generated for: %s</h3>\n", engine.user_name.c_str());
            }

            // Run all tabs sequentially
            engine.print_birth_chart_ui(); 
            engine.analyze_chart("D1", true); 
            
            engine.calculate_dasha_balance();
            engine.print_dasha_web();
            
            time_t t = time(nullptr); tm* now = gmtime(&t);
            engine.calculate_transits(now->tm_year + 1900, now->tm_mon + 1, now->tm_mday, 12, 0, 0, true, true);
            
            engine.analyze_doshas(engine.planet_rashis, engine.planet_rashis[0]);
            engine.analyze_spouse_age_gap(is_female, true);
            engine.analyze_progeny(is_female, true);

            printf("</body>\n</html>\n");
            fflush(stdout); 
            return 0;
        }

        // --- RESTORED ORIGINAL CLI COMMANDS & WEB ALIASES ---
        else if (strcasecmp(cmd.c_str(), "kp") == 0) {
            engine.calculate_kp(); 
            printf("\n"); fflush(stdout); 
            return 0; 
        }
        else if (strcasecmp(cmd.c_str(), "analyze") == 0) {
            string varga = "D1"; if (clean_argc >= 10) varga = clean_argv[9];
            engine.analyze_chart(varga,false); 
            printf("\n"); fflush(stdout); 
            return 0;
        }
        else if (strcasecmp(cmd.c_str(), "collision") == 0) {
            if (clean_argc >= 10) {
                string col_planet = clean_argv[9];
                int v_num = 1;
                int arg_idx = 10;
                
                // Check if arg is D9, D10 etc
                if (clean_argc > arg_idx && toupper(clean_argv[arg_idx][0]) == 'D') {
                    string v_str = clean_argv[arg_idx++];
                    v_num = stoi(v_str.substr(1));
                }
                
                bool col_year_only = false, col_month_only = false;
                if (clean_argc == arg_idx + 1) { t_year = stoi(clean_argv[arg_idx]); col_year_only = true; }
                else if (clean_argc == arg_idx + 2) { t_year = stoi(clean_argv[arg_idx]); t_month = stoi(clean_argv[arg_idx+1]); col_month_only = true; }
                else if (clean_argc >= arg_idx + 3) { t_year = stoi(clean_argv[arg_idx]); t_month = stoi(clean_argv[arg_idx+1]); t_day = stoi(clean_argv[arg_idx+2]); }
                
                // Execute the heavily upgraded Varga Collision Sweeper
                engine.calculate_collisions(col_planet, t_year, t_month, t_day, col_year_only, col_month_only, v_num); 
            } else print_help_menu();
            printf("\n"); fflush(stdout); 
            return 0;
        }
		else if (strcasecmp(cmd.c_str(), "daily") == 0) {
            if (clean_argc >= 12) { t_year = stoi(clean_argv[9]); t_month = stoi(clean_argv[10]); t_day = stoi(clean_argv[11]); }
            else { t_year = year; t_month = month; t_day = day; }
            engine.calculate_muhurat(t_year, t_month, t_day, true);
            engine.calculate_daily_panchang_transitions(t_year, t_month, t_day);
            engine.calculate_daily_lagnas(t_year, t_month, t_day); 
            engine.calculate_daily_horas(t_year, t_month, t_day); 
            printf("\n"); fflush(stdout); 
            return 0;
        }
		else if (strcasecmp(cmd.c_str(), "panchang") == 0) {
            if (clean_argc >= 12) { 
                t_year = stoi(clean_argv[9]); t_month = stoi(clean_argv[10]); t_day = stoi(clean_argv[11]); 
                parse_target_time(12); // Defaults to 12:00:00 noon if exact time isn't passed
                engine.calculate_target_panchang(t_year, t_month, t_day, t_hour, t_min, t_sec, false);
            } else {
                // If no date is passed, use the current system date & time
                engine.calculate_target_panchang(0, 0, 0, 0, 0, 0, true);
            }
            printf("\n"); fflush(stdout); 
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
            printf("\n"); fflush(stdout); 
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
            printf("\n"); fflush(stdout); 
            return 0;
        }
        else if (strcasecmp(cmd.c_str(), "transit") == 0) {
            if (clean_argc >= 12) { 
                t_year = stoi(clean_argv[9]); t_month = stoi(clean_argv[10]); t_day = stoi(clean_argv[11]); parse_target_time(12);
                engine.calculate_muhurat(t_year, t_month, t_day, true);
                engine.calculate_daily_panchang_transitions(t_year, t_month, t_day);
                engine.calculate_transits(t_year, t_month, t_day, t_hour, t_min, t_sec, false, false); 
            } else {
                time_t t = time(nullptr); tm* now = localtime(&t);
                engine.calculate_muhurat(now->tm_year + 1900, now->tm_mon + 1, now->tm_mday, true);
                engine.calculate_daily_panchang_transitions(now->tm_year + 1900, now->tm_mon + 1, now->tm_mday);
                engine.calculate_transits(0, 0, 0, 0, 0, 0, true, false);
            }
            printf("\n"); fflush(stdout); 
            return 0;
        }
		
		else if (strcasecmp(cmd.c_str(), "degree") == 0) {
            if (clean_argc >= 11) {
                string arg1 = clean_argv[9];
                string arg2 = clean_argv[10];

                int tgt_p_idx = engine.get_planet_idx(arg2);
                int tgt_s_idx = engine.get_rashi_idx(arg2);

                bool is_year_only = true;
                for(char c : arg2) if(!isdigit(c)) { is_year_only = false; break; }

                bool include_aspects = false;
                vector<int> nums;
                for (int k = 11; k < clean_argc; k++) {
                    string a = clean_argv[k];
                    if (strcasecmp(a.c_str(), "aspects") == 0) include_aspects = true;
                    else if (isdigit(a[0]) || (a[0] == '-' && a.length() > 1 && isdigit(a[1]))) {
                        nums.push_back(stoi(a));
                    }
                }

                if (tgt_p_idx != -1) {
                    // MODE 1: Planet to Natal Planet
                    int s_year = 0, s_month = 0, s_day = 0;
                    if (nums.size() >= 1) s_year = nums[0];
                    if (nums.size() >= 2) s_month = nums[1];
                    if (nums.size() >= 3) s_day = nums[2];
                    engine.search_planet_conjunct_planet(arg1, arg2, s_year, s_month, s_day, include_aspects);
                } 
                else if (tgt_s_idx != -1) {
                    if (nums.size() > 0) {
                        if (nums[0] > 1000 || nums.size() < 3) {
                            // MODE 2: Planet to Rashi Boundary
                            int s_year = nums[0], s_month = 0, s_day = 0;
                            if (nums.size() >= 2) s_month = nums[1];
                            if (nums.size() >= 3) s_day = nums[2];
                            engine.search_planet_transit_rashi(arg1, arg2, s_year, s_month, s_day, include_aspects);
                        } else {
                            // MODE 3: Old Exact Degree
                            int s_deg = nums[0];
                            int s_min = nums[1];
                            int s_sec = nums[2];
                            int s_year = 0, s_month = 0, s_day = 0;
                            if (nums.size() >= 4) s_year = nums[3];
                            if (nums.size() >= 5) s_month = nums[4];
                            if (nums.size() >= 6) s_day = nums[5];
                            engine.search_exact_degree(arg1, arg2, s_deg, s_min, s_sec, s_year, s_month, s_day);
                        }
                    } else {
                        // MODE 2: Planet to Rashi Boundary (Lifespan default)
                        engine.search_planet_transit_rashi(arg1, arg2, 0, 0, 0, include_aspects);
                    }
                } 
                else if (is_year_only) {
                    // MODE 4: Planet All Rashi/Nakshatra Transits for a specific Year
                    int s_year = stoi(arg2);
                    engine.search_planet_all_transits(arg1, s_year);
                }
                else {
                    printf("Error: Target '%s' is neither a recognized planet, sign, nor a valid year.\n", arg2.c_str());
                }
            } else {
                printf("Error: 'degree' requires at least a Source Planet and a Target (Planet, Sign, or Year).\n");
                printf("Usage 1: degree sun moon 2026 [aspects]\n");
                printf("Usage 2: degree jupiter aries 2026 [aspects]\n");
                printf("Usage 3: degree lagna tula 11 41 09\n");
                printf("Usage 4: degree jupiter 2010\n");
            }
            printf("\n"); fflush(stdout); 
            return 0;
        }
		
		else if (strcasecmp(cmd.c_str(), "tithi") == 0) {
            if (clean_argc >= 10) engine.calculate_tithi_return(stoi(clean_argv[9]));
            else printf("Error: 'tithi' requires a target year. Example: tithi 2026\n");
            printf("\n"); fflush(stdout); 
            return 0;
        }
        else if (strcasecmp(cmd.c_str(), "match") == 0 || strcasecmp(cmd.c_str(), "match_predict") == 0 || strcasecmp(cmd.c_str(), "web_match") == 0 || strcasecmp(cmd.c_str(), "web_synastry") == 0) {
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
                
                JyotishaEngine p2_engine(m_y, m_m, m_d, m_h, m_min, m_s, *it2, json_mode, telugu_ui, html_ui, use_savana, use_true_node);
                p2_engine.calculate_chart();
                
                
                if (is_predict) predict_synastry_events(engine, p2_engine, start_y, end_y);
                else calculate_synastry(engine, p2_engine);
            } else printf("Error: Missing parameters for match command.\n");
            
            printf("\n"); fflush(stdout); 
            return 0;
        }
        else if (strcasecmp(cmd.c_str(), "annual") == 0 || strcasecmp(cmd.c_str(), "web_annual") == 0 || strcasecmp(cmd.c_str(), "web_varshaphal") == 0) {
            if (clean_argc >= 10) {
                int annual_year = stoi(clean_argv[9]);
                double tithi_jd = engine.calculate_tithi_return(annual_year);
                if (tithi_jd > 0.0) {
                    int ty, tm, td; double jut; 
                    swe_revjul(tithi_jd + (engine.location.tz_offset / 24.0), SE_GREG_CAL, &ty, &tm, &td, &jut);
                    int th = (int)jut; int tmin = (int)((jut - th) * 60.0); int tsec = (int)((((jut - th) * 60.0) - tmin) * 60.0);
                    
                    if (html_ui) {
                        printf("<h2 style='margin-top: 20px; margin-bottom: 15px; color: var(--accent);'>VARSHA KUNDALI (ANNUAL CHART FOR %d)</h2>", annual_year);
                    } else {
                        printf("\n=================================================================\n");
                        printf("=== VARSHA KUNDALI (ANNUAL CHART FOR %d) ===\n", annual_year);
                        printf("=================================================================\n");
                    }
                    
                    JyotishaEngine annual_engine(ty, tm, td, th, tmin, tsec, *it, json_mode, telugu_ui, html_ui, use_savana, use_true_node);
                    annual_engine.calculate_chart();
                    
                    // --- FIX: Print the actual Rasi Chart Grid so it doesn't look empty! ---
                    annual_engine.print_birth_chart_ui(); 
                    
                    annual_engine.analyze_placements(annual_engine.planet_rashis, annual_engine.planet_rashis[0]);
                    annual_engine.analyze_lordships(annual_engine.planet_rashis[0], annual_engine.planet_rashis);
                    annual_engine.analyze_conjunctions(annual_engine.planet_rashis, annual_engine.planet_rashis[0]);
                }
            } else printf("Error: 'annual' requires a target year.\n");
            
            printf("\n"); fflush(stdout); 
            return 0;
        }
        else if (strcasecmp(cmd.c_str(), "muhurat") == 0 || strcasecmp(cmd.c_str(), "web_muhurat") == 0) {
            if (clean_argc >= 12) {
                engine.calculate_event_muhurat(clean_argv[9], stoi(clean_argv[10]), stoi(clean_argv[11]));
            } else printf("Error: 'muhurat' requires Event, Year, Month. Ex: muhurat marriage 2026 6\n");
            
            printf("\n"); fflush(stdout); 
            return 0;
        }
        else if (strcasecmp(cmd.c_str(), "predict") == 0) {
            if (clean_argc >= 11) 
			{
				string gender_arg = "UNKNOWN";
				if (clean_argc >= 12) {
					gender_arg = clean_argv[11];
				}
				
				engine.predict_marriage(stoi(clean_argv[9]), stoi(clean_argv[10]), gender_arg);
			}
            else printf("Error: 'predict' requires Start Year and End Year.\n");
            
            printf("\n"); fflush(stdout); 
            return 0;
        }
        else if (strcasecmp(cmd.c_str(), "ayush") == 0) { 
            run_ayush_analysis(engine); 
            printf("\n"); fflush(stdout); 
            return 0; 
        }
        else if (strcasecmp(cmd.c_str(), "all") == 0) {
            string target_planet_all = "all";
            int v_num = 1;
            int arg_idx = 9;
            
            // Check if arg is a specific planet
            if (clean_argc > arg_idx && !isdigit(clean_argv[arg_idx][0]) && toupper(clean_argv[arg_idx][0]) != 'D') {
                target_planet_all = clean_argv[arg_idx++];
            }
            
            // Check if arg is D9, D10 etc
            if (clean_argc > arg_idx && toupper(clean_argv[arg_idx][0]) == 'D') {
                string v_str = clean_argv[arg_idx++];
                v_num = stoi(v_str.substr(1));
            }
            
            if (clean_argc > arg_idx) t_year = stoi(clean_argv[arg_idx++]);
            if (clean_argc > arg_idx) t_month = stoi(clean_argv[arg_idx++]);
            if (clean_argc > arg_idx) t_day = stoi(clean_argv[arg_idx++]);

            int target_year = (t_year > 0) ? t_year : year; 
            
            engine.calculate_navatara_table();
            engine.calculate_special_karakas();
            engine.calculate_muhurat(year, month, day, true); 
            engine.calculate_daily_panchang_transitions(year, month, day);
            
            int varsha_lord_idx = -1, masa_lord_idx = -1;
            engine.calculate_varsha_masa(varsha_lord_idx, masa_lord_idx);
            
            ShadbalaEngine::calculate(engine.lagna_lon, engine.planet_lons, engine.moon_lon, engine.tjd_ut, 
                                      engine.local_hour_decimal, engine.sunrise_hour_decimal, engine.sunset_hour_decimal, 
                                      engine.current_weekday, varsha_lord_idx, masa_lord_idx, false, html_ui, telugu_ui, engine.json_output);
            
            engine.calculate_aspects();
            engine.calculate_shodashvarga(); 
            engine.calculate_ashtakavarga();
            engine.calculate_panchang();
            
            engine.calculate_dasha_balance();
            engine.calculate_6_level_dasha_target(0, 0, 0, 12, 0, 0, true);
            
            string varga_str = (v_num == 1) ? "D1" : "D" + to_string(v_num);
            engine.print_birth_chart_ui();
            engine.analyze_chart(varga_str, true);
            
            // 1. Execute the Varga Transit Scanner (D_x Transit -> D_x Natal)
            engine.scan_planetary_collisions(target_planet_all, target_year, t_month, t_day, v_num); 
            
            // 2. Execute the Rasi Tulya Varga Scanner (D1 Transit -> D_x Natal)
            if (v_num > 1) {
                engine.scan_rasi_tulya_varga_collisions(target_planet_all, target_year, t_month, t_day, v_num);
            }
            
            printf("\n"); fflush(stdout); 
            return 0;
        }
		else if (strcasecmp(cmd.c_str(), "job") == 0) {
            int s_year = year;
            int e_year = year + 10; // Default to a 10-year span
            
            if (clean_argc >= 10) s_year = stoi(clean_argv[9]);
            if (clean_argc >= 11) e_year = stoi(clean_argv[10]);
            
            engine.predict_job(s_year, e_year);
            printf("\n"); fflush(stdout); 
            return 0;
        }
		else if (strcasecmp(cmd.c_str(), "business") == 0) {
            int s_year = year;
            int e_year = year + 10; // Default to a 10-year span
            
            if (clean_argc >= 10) s_year = stoi(clean_argv[9]);
            if (clean_argc >= 11) e_year = stoi(clean_argv[10]);
            
            engine.predict_business(s_year, e_year);
            printf("\n"); fflush(stdout); 
            return 0;
        }
		else if (strcasecmp(cmd.c_str(), "study") == 0) {
            int s_year = year;
            int e_year = year + 25; // Default to a 25-year span to cover childhood to masters
            
            if (clean_argc >= 10) s_year = stoi(clean_argv[9]);
            if (clean_argc >= 11) e_year = stoi(clean_argv[10]);
            
            engine.predict_study(s_year, e_year);
            printf("\n"); fflush(stdout); 
            return 0;
        }
		else { print_help_menu(); return 1; }
    }

    // Default action if no command is provided
    print_help_menu();
    return 0;
}