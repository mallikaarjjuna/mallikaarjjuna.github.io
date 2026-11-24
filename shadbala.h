#ifndef SHADBALA_H
#define SHADBALA_H

#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <cstdio>

extern "C" {
    #include "swephexp.h"
}

using namespace std;

class ShadbalaEngine {
private:
    static double norm360(double lon) {
        double v = fmod(lon, 360.0);
        if (v < 0.0) v += 360.0;
        if (v >= 360.0) v = 0.0;
        return v;
    }

    static double arc_dist(double a, double b) {
        double d = std::abs(norm360(a) - norm360(b));
        return (d > 180.0) ? (360.0 - d) : d;
    }

    // 0-based planet index: 0=Su, 1=Mo, 2=Ma, 3=Me, 4=Ju, 5=Ve, 6=Sa
    static int get_rashi_lord_0(int rashi) {
        static const int lords[12] = {2, 5, 3, 1, 0, 3, 5, 2, 4, 6, 6, 4};
        return lords[(rashi % 12 + 12) % 12];
    }

    // BPHS Ch. 3 Natural Friendship (0-based: Su=0..Sa=6; Shukra -> Guru = 0 Neutral)
    static int get_nat_friend_0(int p, int l) {
        static const int nf[7][7] = {
            { 0,  1,  1,  0,  1, -1, -1}, // 0: Surya
            { 1,  0,  0,  1,  0,  0,  0}, // 1: Chandra
            { 1,  1,  0, -1,  1,  0,  0}, // 2: Mangal
            { 1, -1,  0,  0,  0,  1,  0}, // 3: Budha
            { 1,  1,  1, -1,  0, -1,  0}, // 4: Guru
            {-1, -1,  0,  1,  0,  0,  1}, // 5: Shukra
            {-1, -1, -1,  1,  0,  1,  0}  // 6: Shani
        };
        return nf[p][l];
    }

    // Exact BPHS Ch. 6 Saptavarga calculator (D1, D2, D3, D7, D9, D12, D30)
    static int get_sapta_varga_rashi(int v_num, double lon) {
        double nlon = norm360(lon);
        int r = static_cast<int>(nlon / 30.0);
        if (r > 11) r = 11;
        double deg = nlon - (r * 30.0);
        if (deg < 0.0) deg = 0.0;
        bool is_odd = (r % 2 == 0);

        auto part = [&](int n) {
            int p = static_cast<int>((deg * static_cast<double>(n)) / 30.0);
            if (p < 0) p = 0;
            if (p >= n) p = n - 1;
            return p;
        };

        switch (v_num) {
            case 1:  return r;
            case 2:  return is_odd ? (deg < 15.0 ? 4 : 3) : (deg < 15.0 ? 3 : 4);
            case 3:  return (r + part(3) * 4) % 12;
            case 7:  return (is_odd ? (r + part(7)) : (r + 6 + part(7))) % 12;
            case 9:  { static const int st[4] = {0, 9, 6, 3}; return (st[r % 4] + part(9)) % 12; }
            case 12: return (r + part(12)) % 12;
            case 30:
                if (is_odd) {
                    if (deg < 5.0) return 0; else if (deg < 10.0) return 10;
                    else if (deg < 18.0) return 8; else if (deg < 25.0) return 2;
                    else return 6;
                } else {
                    if (deg < 5.0) return 1; else if (deg < 12.0) return 5;
                    else if (deg < 20.0) return 11; else if (deg < 25.0) return 9;
                    else return 7;
                }
            default: return r;
        }
    }

    // D1 Moolatrikona check per BPHS Ch. 3 (0-based planet index)
    static bool is_d1_moolatrikona(int p, int rashi, double deg) {
        if (p == 0 && rashi == 4  && deg <= 20.0) return true; // Surya: Simha 0-20
        if (p == 1 && rashi == 1  && deg >  3.0)  return true; // Chandra: Vrishabha 3-30
        if (p == 2 && rashi == 0  && deg <= 12.0) return true; // Mangal: Mesha 0-12
        if (p == 3 && rashi == 5  && deg > 15.0 && deg <= 20.0) return true; // Budha: Kanya 15-20
        if (p == 4 && rashi == 8  && deg <= 10.0) return true; // Guru: Dhanu 0-10
        if (p == 5 && rashi == 6  && deg <= 15.0) return true; // Shukra: Tula 0-15
        if (p == 6 && rashi == 10 && deg <= 20.0) return true; // Shani: Kumbha 0-20
        return false;
    }

    // Exact BPHS Ch. 26 Sphuta Drishti (0..60 virupas) from aspector onto aspected point
    static double calc_sphuta_drishti(int aspector_0, double aspector_lon, double aspected_lon) {
        double dk = norm360(aspected_lon - aspector_lon);
        double val = 0.0;

        if (dk >= 30.0 && dk < 60.0)       val = (dk - 30.0) / 2.0;
        else if (dk >= 60.0 && dk < 90.0)  val = (dk - 60.0) + 15.0;
        else if (dk >= 90.0 && dk < 120.0) val = ((120.0 - dk) / 2.0) + 30.0;
        else if (dk >= 120.0 && dk < 150.0) val = (150.0 - dk);
        else if (dk >= 150.0 && dk < 180.0) val = (dk - 150.0) * 2.0;
        else if (dk >= 180.0 && dk < 300.0) val = (300.0 - dk) / 2.0;
        else val = 0.0;

        // Special planetary aspect additions (BPHS Ch. 26)
        if (aspector_0 == 2 && ((dk >= 90.0 && dk < 120.0) || (dk >= 210.0 && dk < 240.0))) val += 15.0; // Mars 4th/8th
        if (aspector_0 == 4 && ((dk >= 120.0 && dk < 150.0) || (dk >= 240.0 && dk < 270.0))) val += 30.0; // Jupiter 5th/9th
        if (aspector_0 == 6 && ((dk >= 60.0 && dk < 90.0) || (dk >= 270.0 && dk < 300.0))) val += 45.0;  // Saturn 3rd/10th

        if (val > 60.0) val = 60.0;
        if (val < 0.0)  val = 0.0;
        return val;
    }

    // BPHS Ch. 27.26-27 Sign Class for Bhava Digbala:
    // 0 = Nara (Human, weak 7H), 1 = Jalachara (Aquatic, weak 10H),
    // 2 = Chatushpada (Quadruped, weak 4H), 3 = Keeta (Insect, weak 1H)
    static int get_bhava_dig_class(int rashi, double deg_in_rashi) {
        switch ((rashi % 12 + 12) % 12) {
            case 2: case 5: case 6: case 10: return 0; // Mithuna, Kanya, Tula, Kumbha -> Nara
            case 3: case 11:                 return 1; // Karka, Meena -> Jalachara
            case 0: case 1: case 4:          return 2; // Mesha, Vrishabha, Simha -> Chatushpada
            case 7:                          return 3; // Vrishchika -> Keeta
            case 8:  return (deg_in_rashi < 15.0) ? 0 : 2; // Dhanu: 0-15 Nara, 15-30 Chatushpada
            case 9:  return (deg_in_rashi < 15.0) ? 2 : 1; // Makara: 0-15 Chatushpada, 15-30 Jalachara
            default: return 0;
        }
    }

public:
    static inline double final_ratios[10]      = {0};
    static inline double final_ishta[10]       = {0};
    static inline double final_kashta[10]      = {0};
    static inline double final_bhava_rupas[13] = {0}; // 1..12
    static inline int    final_bhava_rank[13]  = {0}; // 1..12 (1 = strongest)

    // Preserves exact call signature used in astroc.cpp; optional bhava_madhyas[1..12]
    // allows passing Sripati house midpoints (defaults to Equal-House midpoints from Lagna).
    static void calculate(double lagna_lon, double* planet_lons, double moon_lon, double jd_ut,
                          double local_hour_decimal, double sunrise_hour, double sunset_hour,
                          int weekday, int varsha_lord, int masa_lord,
                          bool json_mode, bool html_mode, bool telugu_mode, string& json_output,
                          const double* bhava_madhyas = nullptr) {
        (void)moon_lon;
        (void)json_output;

        static const int    se_ids[7]         = {SE_SUN, SE_MOON, SE_MARS, SE_MERCURY, SE_JUPITER, SE_VENUS, SE_SATURN};
        static const double exaltation_deg[7] = {10.0, 33.0, 298.0, 165.0, 95.0, 357.0, 200.0};
        // Exact BPHS Naisargika Bala = 60 * k / 7 (Su=7, Mo=6, Ve=5, Ju=4, Me=3, Ma=2, Sa=1)
        static const double naisargika[7]     = {60.0, 360.0/7.0, 120.0/7.0, 180.0/7.0, 240.0/7.0, 300.0/7.0, 60.0/7.0};
        // Exact BPHS Ch. 27 Minimum Shadbala Requirements in Rupas
        static const double min_req[7]        = {6.50, 6.00, 5.00, 7.00, 6.50, 5.50, 5.00};
        // Planetary disc diameters in arcseconds (Surya Siddhanta / B.V. Raman for Yudhdha Bala)
        static const double disc_diam[7]      = {0.0, 0.0, 9.4, 6.6, 190.4, 16.6, 158.0};

        // Self-contained names (shadbala.h is included before texts.h / texts_te.h)
        static const char* sb_rashi_en[12] = {
            "Mesha", "Vrishabha", "Mithuna", "Karka", "Simha", "Kanya",
            "Tula", "Vrishchika", "Dhanu", "Makara", "Kumbha", "Meena"
        };
        static const char* sb_rashi_te[12] = {
            "మేషం", "వృషభం", "మిథునం", "కర్కాటకం", "సింహం", "కన్య",
            "తుల", "వృశ్చికం", "ధనుస్సు", "మకరం", "కుంభం", "మీనం"
        };
        static const char* sb_lord_en[7] = {"Surya", "Chandra", "Mangal", "Budha", "Guru", "Shukra", "Shani"};
        static const char* sb_lord_te[7] = {"సూర్య", "చంద్ర", "కుజ", "బుధ", "గురు", "శుక్ర", "శని"};
        static const char* sb_class_en[4] = {"Nara (Human)", "Jalachara", "Chatushpada", "Keeta"};
        static const char* sb_class_te[4] = {"నర", "జలచర", "చతుష్పాద", "కీట"};

        // Dig Bala zero-strength points (Lagna +/- 90°, matching Jagannatha Hora default)
        double p_weak_pts[7] = {
            norm360(lagna_lon + 90.0),  // Sun: 4th
            norm360(lagna_lon + 270.0), // Moon: 10th
            norm360(lagna_lon + 90.0),  // Mars: 4th
            norm360(lagna_lon + 180.0), // Mercury: 7th
            norm360(lagna_lon + 180.0), // Jupiter: 7th
            norm360(lagna_lon + 270.0), // Venus: 10th
            norm360(lagna_lon)          // Saturn: 1st
        };

        double uchcha[7] = {0}, dig[7] = {0}, saptavargaja[7] = {0}, ojha[7] = {0};
        double kendradi[7] = {0}, drekkana[7] = {0}, sthaana[7] = {0};
        double nathonnatha[7] = {0}, paksha[7] = {0}, tribhaga[7] = {0}, varsha[7] = {0};
        double masa[7] = {0}, dina[7] = {0}, hora[7] = {0}, ayana[7] = {0}, ayana_raw[7] = {0}, yudhdha[7] = {0};
        double kaala[7] = {0}, kaala_no_ayana[7] = {0}, cheshta[7] = {0}, drik[7] = {0};
        double ishta[7] = {0}, kashta[7] = {0};
        double total[7] = {0}, rupas[7] = {0}, ratio[7] = {0};
        int rank[7] = {0};

        // Precompute D1 Rashis & Swiss Ephemeris astronomical quantities
        int d1_rasi[7] = {0};
        double p_lat[7] = {0.0}, p_decl[7] = {0.0}, p_helio[7] = {0.0};
        double ayanamsa = swe_get_ayanamsa_ut(jd_ut);
        int32 sid_flag  = SEFLG_SWIEPH | SEFLG_SIDEREAL | SEFLG_TRUEPOS;

        for (int i = 0; i < 7; i++) {
            double nlon = norm360(planet_lons[i + 1]);
            d1_rasi[i] = static_cast<int>(nlon / 30.0);
            if (d1_rasi[i] > 11) d1_rasi[i] = 11;

            double xx[6]; char serr[256];
            if (swe_calc_ut(jd_ut, se_ids[i], sid_flag, xx, serr) >= 0) {
                p_lat[i] = xx[1];
            }
            if (swe_calc_ut(jd_ut, se_ids[i], SEFLG_SWIEPH | SEFLG_EQUATORIAL, xx, serr) >= 0) {
                p_decl[i] = xx[1];
            } else {
                double trop_lon = norm360(nlon + ayanamsa);
                p_decl[i] = asin(sin(23.4393 * M_PI / 180.0) * sin(trop_lon * M_PI / 180.0)) * 180.0 / M_PI;
            }
            if (i >= 2) {
                if (swe_calc_ut(jd_ut, se_ids[i], sid_flag | SEFLG_HELCTR, xx, serr) >= 0) {
                    p_helio[i] = norm360(xx[0]);
                } else {
                    p_helio[i] = nlon;
                }
            }
        }

        auto panchadha_0 = [&](int p, int l) {
            int nat = get_nat_friend_0(p, l);
            int d = (d1_rasi[l] - d1_rasi[p] + 12) % 12 + 1;
            int temp = (d == 2 || d == 3 || d == 4 || d == 10 || d == 11 || d == 12) ? 1 : -1;
            return nat + temp; // -2..+2
        };

        // ========================================================
        // 1. STHAANA BALA, DIG BALA, AYANA BALA & CHESHTA BALA
        // ========================================================
        static const int sapta_vargas[7] = {1, 2, 3, 7, 9, 12, 30};
        int asc_rasi = static_cast<int>(norm360(lagna_lon) / 30.0) % 12;
        double sun_lon_n = norm360(planet_lons[1]);

        for (int i = 0; i < 7; i++) {
            double lon = norm360(planet_lons[i + 1]);
            int rashi  = d1_rasi[i];
            double deg = lon - (rashi * 30.0);

            // 1a. Uchcha Bala (0..60)
            double dist_ex = arc_dist(lon, exaltation_deg[i]);
            uchcha[i] = (180.0 - dist_ex) / 3.0;

            // 1b. Saptavargaja Bala (Exact BPHS Ch. 27 across D1, D2, D3, D7, D9, D12, D30)
            double sv_sum = 0.0;
            for (int v = 0; v < 7; v++) {
                int vr = get_sapta_varga_rashi(sapta_vargas[v], lon);
                int r_lord = get_rashi_lord_0(vr);

                if (v == 0 && is_d1_moolatrikona(i, rashi, deg)) {
                    sv_sum += 45.0; // Moolatrikona (D1 only)
                } else if (r_lord == i) {
                    sv_sum += 30.0; // Swakshetra (Own Sign)
                } else {
                    int pm = panchadha_0(i, r_lord);
                    if (pm >= 2)       sv_sum += 22.5;  // Adhi-Mitra
                    else if (pm == 1)  sv_sum += 15.0;  // Mitra
                    else if (pm == 0)  sv_sum += 7.5;   // Sama
                    else if (pm == -1) sv_sum += 3.75;  // Shatru
                    else               sv_sum += 1.875; // Adhi-Shatru
                }
            }
            saptavargaja[i] = sv_sum;

            // 1c. Ojhayugmarashiamsha Bala (0, 15, or 30)
            int nav = get_sapta_varga_rashi(9, lon);
            double ojha_score = ((rashi % 2 == 0) ? 15.0 : 0.0) + ((nav % 2 == 0) ? 15.0 : 0.0);
            ojha[i] = (i == 1 || i == 5) ? (30.0 - ojha_score) : ojha_score;

            // 1d. Kendradi Bala (60 / 30 / 15)
            int h_from_asc = (rashi - asc_rasi + 12) % 12 + 1;
            if (h_from_asc % 3 == 1)      kendradi[i] = 60.0;
            else if (h_from_asc % 3 == 2) kendradi[i] = 30.0;
            else                          kendradi[i] = 15.0;

            // 1e. Drekkana Bala (0 or 15: Male 1st, Neutral 2nd, Female 3rd)
            int drek = static_cast<int>(deg / 10.0);
            if (drek > 2) drek = 2;
            if ((i == 0 || i == 2 || i == 4) && drek == 0)      drekkana[i] = 15.0;
            else if ((i == 3 || i == 6) && drek == 1)           drekkana[i] = 15.0;
            else if ((i == 1 || i == 5) && drek == 2)           drekkana[i] = 15.0;

            sthaana[i] = uchcha[i] + saptavargaja[i] + ojha[i] + kendradi[i] + drekkana[i];

            // 2. Dig Bala (0..60)
            dig[i] = arc_dist(lon, p_weak_pts[i]) / 3.0;

            // 3a. Ayana Bala (True Tropical Declination + Directional Sign + Doubled for Sun)
            double kranti = p_decl[i];
            double signed_k = 0.0;
            if (i == 1 || i == 6) signed_k = -kranti;
            else if (i == 3)      signed_k = std::abs(kranti);
            else                  signed_k = kranti;

            double ab = (24.0 + signed_k) * 1.25;
            if (ab < 0.0)  ab = 0.0;
            if (ab > 60.0) ab = 60.0;
            ayana_raw[i] = ab;
            ayana[i] = (i == 0) ? (ab * 2.0) : ab;

            // 4. Cheshta Bala (Exact BPHS Ch. 27.24-25 Cheshta Kendra for Mars..Saturn)
            if (i >= 2) {
                double seeghrocha = (i == 3 || i == 5) ? p_helio[i] : sun_lon_n;
                double mean_lon   = (i == 3 || i == 5) ? sun_lon_n  : p_helio[i];
                double diff_mt    = fmod(lon - mean_lon + 540.0, 360.0) - 180.0;
                double avg_lon    = norm360(mean_lon + diff_mt * 0.5);
                double ck         = arc_dist(seeghrocha, avg_lon);
                cheshta[i]        = ck / 3.0;
            }
        }

        // ========================================================
        // 3B. KAALA BALA SUB-COMPONENTS (PAKSHA, NATHONNATHA, TRIBHAGA, V-M-D-H)
        // ========================================================
        double phase = norm360(planet_lons[2] - planet_lons[1]);
        double moon_paksha = (phase <= 180.0) ? (phase / 3.0) : ((360.0 - phase) / 3.0);
        paksha[1] = moon_paksha * 2.0;
        paksha[0] = 60.0 - moon_paksha; paksha[2] = 60.0 - moon_paksha; paksha[6] = 60.0 - moon_paksha;
        paksha[3] = moon_paksha;        paksha[4] = moon_paksha;        paksha[5] = moon_paksha;

        double solar_noon = (sunrise_hour + sunset_hour) / 2.0;
        double hrs_from_noon = std::abs(local_hour_decimal - solar_noon);
        if (hrs_from_noon > 12.0) hrs_from_noon = 24.0 - hrs_from_noon;
        double diurnal   = ((12.0 - hrs_from_noon) / 12.0) * 60.0;
        double nocturnal = 60.0 - diurnal;
        nathonnatha[0] = diurnal;   nathonnatha[4] = diurnal;   nathonnatha[5] = diurnal;
        nathonnatha[1] = nocturnal; nathonnatha[2] = nocturnal; nathonnatha[6] = nocturnal;
        nathonnatha[3] = 60.0;

        int vedic_wday = (local_hour_decimal < sunrise_hour) ? (weekday + 6) % 7 : weekday;
        if (varsha_lord >= 0 && varsha_lord <= 6) varsha[varsha_lord] = 15.0;
        if (masa_lord >= 0 && masa_lord <= 6)     masa[masa_lord]     = 30.0;
        if (vedic_wday >= 0 && vedic_wday <= 6)   dina[vedic_wday]    = 45.0;

        static const int hora_seq[7] = {0, 5, 3, 1, 6, 4, 2};
        int day_lord_idx = 0;
        for (int i = 0; i < 7; i++) { if (hora_seq[i] == vedic_wday) day_lord_idx = i; }

        bool is_day = (local_hour_decimal >= sunrise_hour && local_hour_decimal < sunset_hour);
        double day_len   = sunset_hour - sunrise_hour;
        double night_len = 24.0 - day_len;
        int hora_idx = 0;
        if (is_day && day_len > 0.0) {
            hora_idx = static_cast<int>(((local_hour_decimal - sunrise_hour) / day_len) * 12.0);
            if (hora_idx > 11) hora_idx = 11;
        } else if (night_len > 0.0) {
            double hrs_since_sunset = local_hour_decimal - sunset_hour;
            if (hrs_since_sunset < 0.0) hrs_since_sunset += 24.0;
            hora_idx = 12 + static_cast<int>((hrs_since_sunset / night_len) * 12.0);
            if (hora_idx > 23) hora_idx = 23;
        }
        int hora_lord = hora_seq[(day_lord_idx + hora_idx) % 7];
        hora[hora_lord] = 60.0;

        tribhaga[4] = 60.0;
        if (is_day) {
            double third = day_len / 3.0;
            if (local_hour_decimal < sunrise_hour + third)            tribhaga[3] = 60.0;
            else if (local_hour_decimal < sunrise_hour + 2.0 * third) tribhaga[0] = 60.0;
            else                                                      tribhaga[6] = 60.0;
        } else {
            double third = night_len / 3.0;
            double hrs_since_sunset = local_hour_decimal - sunset_hour;
            if (hrs_since_sunset < 0.0) hrs_since_sunset += 24.0;
            if (hrs_since_sunset < third)            tribhaga[1] = 60.0;
            else if (hrs_since_sunset < 2.0 * third) tribhaga[5] = 60.0;
            else                                     tribhaga[2] = 60.0;
        }

        for (int i = 0; i < 7; i++) {
            kaala_no_ayana[i] = nathonnatha[i] + paksha[i] + tribhaga[i] + varsha[i] + masa[i] + dina[i] + hora[i];
        }

        // ========================================================
        // 3C. YUDHDHA BALA (PLANETARY WAR — BPHS CH. 27.20 / SURYA SIDDHANTA)
        // ========================================================
        for (int i = 2; i < 7; i++) {
            for (int j = i + 1; j < 7; j++) {
                if (arc_dist(planet_lons[i + 1], planet_lons[j + 1]) <= 1.0) {
                    double bala_i = sthaana[i] + dig[i] + kaala_no_ayana[i];
                    double bala_j = sthaana[j] + dig[j] + kaala_no_ayana[j];
                    double diam_diff = std::abs(disc_diam[i] - disc_diam[j]);
                    double adj = (diam_diff > 0.01) ? (std::abs(bala_i - bala_j) / diam_diff) : std::abs(bala_i - bala_j);

                    double signed_diff = fmod(planet_lons[j + 1] - planet_lons[i + 1] + 540.0, 360.0) - 180.0;
                    bool i_wins = (std::abs(p_lat[i] - p_lat[j]) > 1e-6) ? (p_lat[i] > p_lat[j]) : (signed_diff > 0.0);

                    if (i_wins) { yudhdha[i] += adj; yudhdha[j] -= adj; }
                    else        { yudhdha[j] += adj; yudhdha[i] -= adj; }
                }
            }
        }

        // ========================================================
        // 5. DRIK BALA (EXACT BPHS CH. 26 SPHUTA DRISHTI)
        // ========================================================
        bool moon_is_ben = (phase >= 72.0 && phase <= 288.0);
        bool merc_is_ben = true;
        for (int k = 0; k < 7; k++) {
            if (k == 3 || d1_rasi[k] != d1_rasi[3]) continue;
            if (k == 0 || k == 2 || k == 6 || (k == 1 && !moon_is_ben)) {
                merc_is_ben = false;
                break;
            }
        }
        bool is_ben[7] = {false, moon_is_ben, false, merc_is_ben, true, true, false};

        for (int target = 0; target < 7; target++) {
            double net_drishti = 0.0;
            for (int aspector = 0; aspector < 7; aspector++) {
                if (aspector == target) continue;
                double d_val = calc_sphuta_drishti(aspector, planet_lons[aspector + 1], planet_lons[target + 1]);
                if (is_ben[aspector]) net_drishti += d_val;
                else                  net_drishti -= d_val;
            }
            drik[target] = net_drishti / 4.0;
        }

        // ========================================================
        // 6. FINAL SHADBALA AGGREGATES, ISHTA/KASHTA & RATIOS
        // ========================================================
        for (int i = 0; i < 7; i++) {
            kaala[i] = kaala_no_ayana[i] + ayana[i] + yudhdha[i];
            total[i] = sthaana[i] + dig[i] + kaala[i] + cheshta[i] + naisargika[i] + drik[i];
            rupas[i] = total[i] / 60.0;
            ratio[i] = rupas[i] / min_req[i];
            final_ratios[i + 1] = ratio[i];

            double ch_eff = (i == 0) ? ayana_raw[0] : (i == 1) ? moon_paksha : cheshta[i];
            if (ch_eff < 0.0)  ch_eff = 0.0;
            if (ch_eff > 60.0) ch_eff = 60.0;
            ishta[i]  = std::sqrt(uchcha[i] * ch_eff);
            kashta[i] = std::sqrt((60.0 - uchcha[i]) * (60.0 - ch_eff));
            final_ishta[i + 1]  = ishta[i];
            final_kashta[i + 1] = kashta[i];
        }

        for (int i = 0; i < 7; i++) {
            rank[i] = 1;
            for (int j = 0; j < 7; j++) { if (rupas[j] > rupas[i]) rank[i]++; }
        }

        // ========================================================
        // 7. CLASSICAL BPHS CH. 27 BHAVA BALA (12 HOUSES)
        // ========================================================
        double bhava_lon[12]     = {0.0};
        int    bhava_rasi[12]    = {0};
        double bhava_deg[12]     = {0.0};
        int    bhava_lord[12]    = {0};
        int    bhava_cls[12]     = {0};
        double bhavadhipati[12]  = {0.0};
        double bhava_dig[12]     = {0.0};
        double bhava_drishti[12] = {0.0};
        double bhava_total[12]   = {0.0};
        double bhava_rupas[12]   = {0.0};
        int    bhava_rank[12]    = {0};

        // Weak house (1..12) by sign class: Nara=7, Jalachara=10, Chatushpada=4, Keeta=1
        static const int weak_house_for_class[4] = {7, 10, 4, 1};

        for (int h = 0; h < 12; h++) {
            int h_num = h + 1;
            bhava_lon[h]  = (bhava_madhyas != nullptr) ? norm360(bhava_madhyas[h_num])
                                                       : norm360(lagna_lon + h * 30.0);
            bhava_rasi[h] = static_cast<int>(bhava_lon[h] / 30.0);
            if (bhava_rasi[h] > 11) bhava_rasi[h] = 11;
            bhava_deg[h]  = bhava_lon[h] - (bhava_rasi[h] * 30.0);

            // 7a. Bhavadhipati Bala (Lord's Total Shadbala in Virupas)
            bhava_lord[h]   = get_rashi_lord_0(bhava_rasi[h]);
            bhavadhipati[h] = total[bhava_lord[h]];

            // 7b. Bhava Digbala (0..60 Virupas per BPHS Ch. 27.26-27)
            bhava_cls[h] = get_bhava_dig_class(bhava_rasi[h], bhava_deg[h]);
            int dist_h   = std::abs(h_num - weak_house_for_class[bhava_cls[h]]);
            if (dist_h > 6) dist_h = 12 - dist_h;
            bhava_dig[h] = dist_h * 10.0;

            // 7c. Bhava Drishti Bala (BPHS Ch. 27.28-29)
            // Guru(4) & Budha(3): +100% | Shukra(5) & Waxing Moon(1): +25% | Sun(0), Mars(2), Saturn(6), Waning Moon: -25%
            double bd = 0.0;
            for (int aspector = 0; aspector < 7; aspector++) {
                double d_val = calc_sphuta_drishti(aspector, planet_lons[aspector + 1], bhava_lon[h]);
                if (aspector == 3 || aspector == 4) {
                    bd += d_val;
                } else if (aspector == 5 || (aspector == 1 && moon_is_ben)) {
                    bd += (d_val / 4.0);
                } else {
                    bd -= (d_val / 4.0);
                }
            }
            bhava_drishti[h] = bd;

            bhava_total[h] = bhavadhipati[h] + bhava_dig[h] + bhava_drishti[h];
            bhava_rupas[h] = bhava_total[h] / 60.0;
            final_bhava_rupas[h_num] = bhava_rupas[h];
        }

        for (int h = 0; h < 12; h++) {
            bhava_rank[h] = 1;
            for (int k = 0; k < 12; k++) {
                if (bhava_total[k] > bhava_total[h]) bhava_rank[h]++;
            }
            final_bhava_rank[h + 1] = bhava_rank[h];
        }

        // ========================================================
        // 8. OUTPUT TABLES (SHADBALA + BHAVA BALA)
        // ========================================================
        if (!json_mode) {
            if (html_mode) {
                printf("<h2 style='margin-top: 30px; margin-bottom: 10px; color: var(--accent);'>%s</h2>\n",
                       telugu_mode ? "షడ్బల విశ్లేషణ (Shadbala)" : "Comprehensive Shadbala");
                printf("<table class='data-table'>\n");
                printf("<tr><th>%s</th><th>Moon</th><th>Sun</th><th>Mercury</th><th>Venus</th><th>Mars</th><th>Jupiter</th><th>Saturn</th></tr>\n",
                       telugu_mode ? "బలాలు (Balas)" : "Balas");

                auto p_row_html = [](const char* name, const double* v) {
                    printf("<tr><td><b>%s</b></td><td>%.2f</td><td>%.2f</td><td>%.2f</td><td>%.2f</td><td>%.2f</td><td>%.2f</td><td>%.2f</td></tr>\n",
                           name, v[1], v[0], v[3], v[5], v[2], v[4], v[6]);
                };
                auto p_int_html = [](const char* name, const int* v) {
                    printf("<tr><td><b>%s</b></td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td></tr>\n",
                           name, v[1], v[0], v[3], v[5], v[2], v[4], v[6]);
                };

                p_row_html("Uchcha Bala", uchcha);
                p_row_html("Saptavargaja Bala", saptavargaja);
                p_row_html("Ojhayugmarasiamsa Bala", ojha);
                p_row_html("Kendradi Bala", kendradi);
                p_row_html("Drekkana Bala", drekkana);
                p_row_html("Sthaana Bala", sthaana);
                p_row_html("Dig Bala", dig);
                p_row_html("Nathonnatha Bala", nathonnatha);
                p_row_html("Paksha Bala", paksha);
                p_row_html("Tribhaga Bala", tribhaga);
                p_row_html("Varsha Bala", varsha);
                p_row_html("Masa Bala", masa);
                p_row_html("Dina Bala", dina);
                p_row_html("Hora Bala", hora);
                p_row_html("Ayana Bala", ayana);
                p_row_html("Yudhdha Bala", yudhdha);
                p_row_html("Kaala Bala", kaala);
                p_row_html("Cheshta Bala", cheshta);
                p_row_html("Naisargika Bala", naisargika);
                p_row_html("Drik Bala", drik);

                printf("<tr style='background:#3a3a45; color:var(--term-text); border-top:2px solid var(--border);'><td colspan='8'><b>Final Aggregates</b></td></tr>\n");
                p_row_html("Total Shadbala", total);
                p_row_html("Shadbala in Rupas", rupas);
                p_row_html("Minimum Requirement", min_req);

                printf("<tr style='background:#2a2a35;'><td><b>Ratio</b></td>");
                for (int k = 0; k < 7; k++) {
                    int p = (k==0)?1 : (k==1)?0 : (k==2)?3 : (k==3)?5 : (k==4)?2 : (k==5)?4 : 6;
                    const char* color = (ratio[p] >= 1.0) ? "#2ecc71" : "#e74c3c";
                    printf("<td><b style='color:%s;'>%.2f</b></td>", color, ratio[p]);
                }
                printf("</tr>\n");

                p_int_html("Relative Rank", rank);
                p_row_html("Ishta Phala", ishta);
                p_row_html("Kashta Phala", kashta);
                printf("</table>\n");

                // --- HTML BHAVA BALA TABLE ---
                printf("<h2 style='margin-top: 30px; margin-bottom: 10px; color: var(--accent);'>%s</h2>\n",
                       telugu_mode ? "భావ బల విశ్లేషణ (BPHS Bhava Bala)" : "Bhava Bala (12-House Strength — BPHS Ch. 27)");
                printf("<table class='data-table'>\n");
                printf("<tr><th>%s</th><th>%s</th><th>%s</th><th>%s</th><th>%s</th><th>%s</th><th>%s</th><th>%s</th><th>%s</th><th>%s</th></tr>\n",
                       telugu_mode ? "భావం" : "House",
                       telugu_mode ? "మధ్య బిందువు" : "Midpoint",
                       telugu_mode ? "అధిపతి" : "Lord",
                       telugu_mode ? "రాశి వర్గం" : "Class",
                       telugu_mode ? "భావాధిపతి బలం" : "Bhavadhipati",
                       telugu_mode ? "భావ దిగ్బలం" : "Bhava Dig",
                       telugu_mode ? "భావ దృష్టి బలం" : "Bhava Drishti",
                       telugu_mode ? "మొత్తం (విరూప)" : "Total (Vir)",
                       telugu_mode ? "రూపాలు" : "Rupas",
                       telugu_mode ? "ర్యాంక్" : "Rank");

                for (int h = 0; h < 12; h++) {
                    int deg_d = static_cast<int>(bhava_deg[h]);
                    int deg_m = static_cast<int>((bhava_deg[h] - deg_d) * 60.0);
                    char mid_buf[64];
                    snprintf(mid_buf, sizeof(mid_buf), "%02d&deg;%02d' %s",
                             deg_d, deg_m, telugu_mode ? sb_rashi_te[bhava_rasi[h]] : sb_rashi_en[bhava_rasi[h]]);

                    const char* r_col = (bhava_rank[h] <= 4) ? "#2ecc71" : (bhava_rank[h] <= 8) ? "#f1c40f" : "#e74c3c";
                    char h_lbl[32];
                    if (telugu_mode) snprintf(h_lbl, sizeof(h_lbl), "%dవ భావం", h + 1);
                    else             snprintf(h_lbl, sizeof(h_lbl), "House %02d", h + 1);

                    printf("<tr><td><b>%s</b></td><td>%s</td><td>%s</td><td>%s</td><td>%.2f</td><td>%.2f</td><td>%+.2f</td><td><b>%.2f</b></td><td style='color:%s; font-weight:bold;'>%.2f</td><td style='color:%s; font-weight:bold;'>#%d</td></tr>\n",
                           h_lbl, mid_buf,
                           telugu_mode ? sb_lord_te[bhava_lord[h]] : sb_lord_en[bhava_lord[h]],
                           telugu_mode ? sb_class_te[bhava_cls[h]] : sb_class_en[bhava_cls[h]],
                           bhavadhipati[h], bhava_dig[h], bhava_drishti[h],
                           bhava_total[h], r_col, bhava_rupas[h], r_col, bhava_rank[h]);
                }
                printf("</table>\n");
                printf("<p style='font-size:12px; color:#888; margin-top:6px;'>%s</p>\n",
                       telugu_mode
                           ? "గమనిక: భావ బలం = భావాధిపతి బలం + భావ దిగ్బలం + భావ దృష్టి బలం (BPHS Ch. 27). గురు/బుధ దృష్టి +100%, శుభ +25%, పాప -25%."
                           : "Note: Bhava Bala = Bhavadhipati + Bhava Digbala + Bhava Drishti Bala (BPHS Ch. 27; Equal-House midpoints). Guru/Budha aspect +100%, other benefics +25%, malefics -25%.");
            } else {
                printf("\n=== COMPREHENSIVE SHADBALA TABLE ===\n");
                printf("--------------------------------------------------------------------------------------------------\n");
                printf("%-25s | %-8s | %-8s | %-8s | %-8s | %-8s | %-8s | %-8s\n", "Balas", "Moon", "Sun", "Mercury", "Venus", "Mars", "Jupiter", "Saturn");
                printf("--------------------------------------------------------------------------------------------------\n");

                auto p_row = [](const char* name, const double* v) {
                    printf("%-25s | %-8.2f | %-8.2f | %-8.2f | %-8.2f | %-8.2f | %-8.2f | %-8.2f\n",
                           name, v[1], v[0], v[3], v[5], v[2], v[4], v[6]);
                };
                auto p_int = [](const char* name, const int* v) {
                    printf("%-25s | %-8d | %-8d | %-8d | %-8d | %-8d | %-8d | %-8d\n",
                           name, v[1], v[0], v[3], v[5], v[2], v[4], v[6]);
                };

                p_row("Uchcha Bala", uchcha);
                p_row("Saptavargaja Bala", saptavargaja);
                p_row("Ojhayugmarasiamsa Bala", ojha);
                p_row("Kendradi Bala", kendradi);
                p_row("Drekkana Bala", drekkana);
                p_row("Sthaana Bala", sthaana);
                p_row("Dig Bala", dig);
                p_row("Nathonnatha Bala", nathonnatha);
                p_row("Paksha Bala", paksha);
                p_row("Tribhaga Bala", tribhaga);
                p_row("Varsha Bala", varsha);
                p_row("Masa Bala", masa);
                p_row("Dina Bala", dina);
                p_row("Hora Bala", hora);
                p_row("Ayana Bala", ayana);
                p_row("Yudhdha Bala", yudhdha);
                p_row("Kaala Bala", kaala);
                p_row("Cheshta Bala", cheshta);
                p_row("Naisargika Bala", naisargika);
                p_row("Drik Bala", drik);
                printf("--------------------------------------------------------------------------------------------------\n");
                p_row("Total Shadbala", total);
                p_row("Shadbala in Rupas", rupas);
                p_row("Minimum Requirement", min_req);
                p_row("Ratio", ratio);
                p_int("Relative Rank", rank);
                p_row("Ishta Phala", ishta);
                p_row("Kashta Phala", kashta);
                printf("--------------------------------------------------------------------------------------------------\n");

                // --- CLI BHAVA BALA TABLE ---
                printf("\n=== %s ===\n",
                       telugu_mode ? "భావ బల పట్టిక (BHAVA BALA — BPHS CH. 27)"
                                   : "BHAVA BALA TABLE (12-HOUSE STRENGTH — BPHS CH. 27)");
                printf("----------------------------------------------------------------------------------------------------------------------------\n");
                printf("%-8s | %-18s | %-8s | %-12s | %-12s | %-9s | %-11s | %-10s | %-6s | %-4s\n",
                       "House", "Midpoint", "Lord", "Class", "Bhavadhipati", "Bhava Dig", "Bhava Dris", "Total(Vir)", "Rupas", "Rank");
                printf("----------------------------------------------------------------------------------------------------------------------------\n");

                for (int h = 0; h < 12; h++) {
                    int deg_d = static_cast<int>(bhava_deg[h]);
                    int deg_m = static_cast<int>((bhava_deg[h] - deg_d) * 60.0);
                    char mid_buf[64];
                    snprintf(mid_buf, sizeof(mid_buf), "%02d°%02d' %s",
                             deg_d, deg_m, telugu_mode ? sb_rashi_te[bhava_rasi[h]] : sb_rashi_en[bhava_rasi[h]]);

                    const char* ansi = (bhava_rank[h] <= 4) ? "\033[1;32m" : (bhava_rank[h] <= 8) ? "\033[1;33m" : "\033[1;31m";

                    printf("House %02d | %-18s | %-8s | %-12s | %12.2f | %9.2f | %+11.2f | %10.2f | %s%6.2f\033[0m | %s#%-3d\033[0m\n",
                           h + 1, mid_buf,
                           telugu_mode ? sb_lord_te[bhava_lord[h]] : sb_lord_en[bhava_lord[h]],
                           telugu_mode ? sb_class_te[bhava_cls[h]] : sb_class_en[bhava_cls[h]],
                           bhavadhipati[h], bhava_dig[h], bhava_drishti[h],
                           bhava_total[h], ansi, bhava_rupas[h], ansi, bhava_rank[h]);
                }
                printf("----------------------------------------------------------------------------------------------------------------------------\n");
                printf("Note: Bhava Bala = Bhavadhipati + Bhava Digbala + Bhava Drishti Bala (BPHS Ch. 27, Equal-House Madhyas).\n");
                printf("      Drishti weights: Guru/Budha +100%% | Venus/Waxing Moon +25%% | Sun/Mars/Saturn/Waning Moon -25%%.\n");
                printf("----------------------------------------------------------------------------------------------------------------------------\n");
            }
        }
    }
};
#endif