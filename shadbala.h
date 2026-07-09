#ifndef SHADBALA_H
#define SHADBALA_H

#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <cstdio>

using namespace std;

class ShadbalaEngine {
public:
    static inline double final_ratios[10] = {0};
    
    // NEW SIGNATURE: Added html_mode and telugu_mode
    static void calculate(double lagna_lon, double* planet_lons, double moon_lon, double jd_ut, double local_hour_decimal, double sunrise_hour, double sunset_hour, int weekday, int varsha_lord, int masa_lord, bool json_mode, bool html_mode, bool telugu_mode, string& json_output) {
        const char* p_names[] = {"Surya", "Chandra", "Mangal", "Budha", "Guru", "Shukra", "Shani"};
        double exaltation_deg[] = {10.0, 33.0, 298.0, 165.0, 95.0, 357.0, 200.0};
        double naisargika[] = {60.0, 51.43, 17.14, 25.70, 34.28, 42.85, 8.57};
        double min_req[] = {5.00, 6.00, 5.00, 7.00, 6.50, 5.50, 5.00};
        
        double p_weak_pts[] = {
            fmod(lagna_lon + 90.0, 360.0), fmod(lagna_lon + 270.0, 360.0), fmod(lagna_lon + 90.0, 360.0),
            fmod(lagna_lon + 180.0, 360.0), fmod(lagna_lon + 180.0, 360.0), fmod(lagna_lon + 270.0, 360.0), lagna_lon
        };

        double uchcha[7] = {0}, dig[7] = {0}, saptavargaja[7] = {0}, ojha[7] = {0};
        double kendradi[7] = {0}, drekkana[7] = {0}, sthaana[7] = {0};
        double nathonnatha[7] = {0}, paksha[7] = {0}, tribhaga[7] = {0}, varsha[7] = {0};
        double masa[7] = {0}, dina[7] = {0}, hora[7] = {0}, ayana[7] = {0}, yudhdha[7] = {0};
        double kaala[7] = {0}, cheshta[7] = {0}, drik[7] = {0}, ishta[7] = {0}, kashta[7] = {0};
        double total[7] = {0}, rupas[7] = {0}, ratio[7] = {0};
        int rank[7] = {0};

        for (int i = 0; i < 7; i++) {
            double dist_ex = abs(planet_lons[i+1] - exaltation_deg[i]);
            if (dist_ex > 180.0) dist_ex = 360.0 - dist_ex;
            uchcha[i] = (180.0 - dist_ex) / 3.0;

            double dist_wk = abs(planet_lons[i+1] - p_weak_pts[i]);
            if (dist_wk > 180.0) dist_wk = 360.0 - dist_wk;
            dig[i] = dist_wk / 3.0;

            int rashi = int(planet_lons[i+1] / 30.0);
            int nav = int(planet_lons[i+1] / (10.0/3.0)) % 12;
            double ojha_score = ((rashi % 2 == 0) ? 15.0 : 0) + ((nav % 2 == 0) ? 15.0 : 0);
            ojha[i] = (i == 1 || i == 5) ? (30.0 - ojha_score) : ojha_score;

            int h_from_asc = (rashi - int(lagna_lon / 30.0) + 12) % 12 + 1;
            if (h_from_asc % 3 == 1) kendradi[i] = 60.0;
            else if (h_from_asc % 3 == 2) kendradi[i] = 30.0;
            else kendradi[i] = 15.0;

            int drek = int(fmod(planet_lons[i+1], 30.0) / 10.0);
            if ((i == 0 || i == 2 || i == 4) && drek == 0) drekkana[i] = 15.0; 
            else if ((i == 3 || i == 6) && drek == 1) drekkana[i] = 15.0; 
            else if ((i == 1 || i == 5) && drek == 2) drekkana[i] = 15.0; 

            saptavargaja[i] = 60.0 + (uchcha[i] * 1.5);
            double declination = 23.45 * sin(planet_lons[i+1] * M_PI / 180.0);
            ayana[i] = abs((24.0 + declination) * 1.25);

            if (i != 0 && i != 1) {
                double elongation = abs(planet_lons[i+1] - planet_lons[1]);
                if (elongation > 180.0) elongation = 360.0 - elongation;
                cheshta[i] = (elongation / 180.0) * 60.0;
            }
            drik[i] = -11.20; 
            ishta[i] = sqrt(uchcha[i] * (cheshta[i] == 0 ? 30.0 : cheshta[i]));
            kashta[i] = sqrt((60.0 - uchcha[i]) * (60.0 - (cheshta[i] == 0 ? 30.0 : cheshta[i])));
        }

        // Planetary War (Yudhdha Bala)
        for (int i = 2; i < 7; i++) {
            for (int j = i + 1; j < 7; j++) {
                if (abs(planet_lons[i+1] - planet_lons[j+1]) <= 1.0) {
                    double diff = abs(uchcha[i] - uchcha[j]);
                    if (planet_lons[i+1] < planet_lons[j+1]) { yudhdha[i] += diff; yudhdha[j] -= diff; }
                    else { yudhdha[j] += diff; yudhdha[i] -= diff; }
                }
            }
        }

        // Paksha Bala
        double phase = fmod(planet_lons[2] - planet_lons[1] + 360.0, 360.0);
        double moon_paksha = (phase < 180.0) ? (phase / 180.0) * 60.0 : ((360.0 - phase) / 180.0) * 60.0;
        paksha[1] = moon_paksha * 2.0; 
        paksha[0] = 60.0 - moon_paksha; paksha[2] = 60.0 - moon_paksha; paksha[6] = 60.0 - moon_paksha;
        paksha[3] = moon_paksha; paksha[4] = moon_paksha; paksha[5] = moon_paksha;

        // Nathonnatha Bala
        double sun_angle = fmod(planet_lons[1] - lagna_lon + 360.0, 360.0);
        double dist_from_midnight = abs(sun_angle - 90.0); 
        if (dist_from_midnight > 180.0) dist_from_midnight = 360.0 - dist_from_midnight;
        double nocturnal = ((180.0 - dist_from_midnight) / 180.0) * 60.0;
        double diurnal = 60.0 - nocturnal;
        nathonnatha[1] = nocturnal; nathonnatha[2] = nocturnal; nathonnatha[6] = nocturnal;
        nathonnatha[0] = diurnal; nathonnatha[4] = diurnal; nathonnatha[5] = diurnal; nathonnatha[3] = 60.0;

        // KAALA BALA Additions
        if (varsha_lord >= 0) varsha[varsha_lord] = 15.0;
        if (masa_lord >= 0) masa[masa_lord] = 30.0;
        
        // Dina Bala (Weekday mapping: 0=Sun, 1=Moon, 2=Mars, 3=Mer, 4=Jup, 5=Ven, 6=Sat)
        if (weekday >= 0 && weekday <= 6) dina[weekday] = 45.0;

        // Hora Bala
        int hora_seq[] = {0, 5, 3, 1, 6, 4, 2}; 
        int day_lord_idx = 0;
        for (int i=0; i<7; i++) { if (hora_seq[i] == weekday) day_lord_idx = i; }
        double hours_passed = local_hour_decimal - sunrise_hour;
        if (hours_passed < 0) hours_passed += 24.0;
        int hora_lord = hora_seq[(day_lord_idx + int(hours_passed)) % 7];
        hora[hora_lord] = 60.0;

        // Tribhaga Bala
        tribhaga[4] = 60.0; 
        bool is_day = (local_hour_decimal >= sunrise_hour && local_hour_decimal < sunset_hour);
        if (is_day) {
            double third = (sunset_hour - sunrise_hour) / 3.0;
            if (local_hour_decimal < sunrise_hour + third) tribhaga[3] = 60.0; 
            else if (local_hour_decimal < sunrise_hour + 2*third) tribhaga[0] = 60.0; 
            else tribhaga[6] = 60.0; 
        } else {
            double night_length = 24.0 - (sunset_hour - sunrise_hour);
            double third = night_length / 3.0;
            double hours_since_sunset = local_hour_decimal - sunset_hour;
            if (hours_since_sunset < 0) hours_since_sunset += 24.0;
            if (hours_since_sunset < third) tribhaga[1] = 60.0; 
            else if (hours_since_sunset < 2*third) tribhaga[5] = 60.0; 
            else tribhaga[2] = 60.0; 
        }

        // Compile Aggregates
        for (int i = 0; i < 7; i++) {
            sthaana[i] = uchcha[i] + saptavargaja[i] + ojha[i] + kendradi[i] + drekkana[i];
            kaala[i] = nathonnatha[i] + paksha[i] + tribhaga[i] + varsha[i] + masa[i] + dina[i] + hora[i] + ayana[i] + yudhdha[i];
            total[i] = sthaana[i] + dig[i] + kaala[i] + cheshta[i] + naisargika[i] + drik[i];
            rupas[i] = total[i] / 60.0;
            ratio[i] = rupas[i] / min_req[i];
            final_ratios[i+1] = ratio[i];
        }

        for (int i = 0; i < 7; i++) {
            rank[i] = 1;
            for (int j = 0; j < 7; j++) { if (rupas[j] > rupas[i]) rank[i]++; }
        }

        if (!json_mode) {
            if (html_mode) {
                printf("<h2 style='margin-top: 30px; margin-bottom: 10px; color: var(--accent);'>%s</h2>\n", telugu_mode ? "షడ్బల విశ్లేషణ (Shadbala)" : "Comprehensive Shadbala");
                printf("<table class='data-table'>\n");
                printf("<tr><th>%s</th><th>Moon</th><th>Sun</th><th>Mercury</th><th>Venus</th><th>Mars</th><th>Jupiter</th><th>Saturn</th></tr>\n", telugu_mode ? "బలాలు (Balas)" : "Balas");
                
                auto p_row_html = [](const char* name, double* v) { 
                    printf("<tr><td><b>%s</b></td><td>%.2f</td><td>%.2f</td><td>%.2f</td><td>%.2f</td><td>%.2f</td><td>%.2f</td><td>%.2f</td></tr>\n", name, v[1], v[0], v[3], v[5], v[2], v[4], v[6]); 
                };
                auto p_int_html = [](const char* name, int* v) { 
                    printf("<tr><td><b>%s</b></td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td><td>%d</td></tr>\n", name, v[1], v[0], v[3], v[5], v[2], v[4], v[6]); 
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
                
                // Color-coded Ratio row
                printf("<tr style='background:#2a2a35;'><td><b>Ratio</b></td>");
                for(int k=0; k<7; k++) {
                    int p = (k==0)?1 : (k==1)?0 : (k==2)?3 : (k==3)?5 : (k==4)?2 : (k==5)?4 : 6;
                    const char* color = (ratio[p] >= 1.0) ? "#2ecc71" : "#e74c3c";
                    printf("<td><b style='color:%s;'>%.2f</b></td>", color, ratio[p]);
                }
                printf("</tr>\n");
                
                p_int_html("Relative Rank", rank);
                p_row_html("Ishta Phala", ishta);
                p_row_html("Kashta Phala", kashta);
                printf("</table>\n");
            } else {
                printf("\n=== COMPREHENSIVE SHADBALA TABLE ===\n");
                printf("--------------------------------------------------------------------------------------------------\n");
                printf("%-25s | %-8s | %-8s | %-8s | %-8s | %-8s | %-8s | %-8s\n", "Balas", "Moon", "Sun", "Mercury", "Venus", "Mars", "Jupiter", "Saturn");
                printf("--------------------------------------------------------------------------------------------------\n");
                
                auto p_row = [](const char* name, double* v) { printf("%-25s | %-8.2f | %-8.2f | %-8.2f | %-8.2f | %-8.2f | %-8.2f | %-8.2f\n", name, v[1], v[0], v[3], v[5], v[2], v[4], v[6]); };
                auto p_int = [](const char* name, int* v) { printf("%-25s | %-8d | %-8d | %-8d | %-8d | %-8d | %-8d | %-8d\n", name, v[1], v[0], v[3], v[5], v[2], v[4], v[6]); };

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
                p_row("Total shadbala Bala", total);
                p_row("Shadbala in rupas", rupas);
                p_row("minimum requirement", min_req);
                p_row("Ratio", ratio);
                p_int("Relative rank", rank);
                p_row("Ishta Phala", ishta);
                p_row("Kashta Phala", kashta);
                printf("--------------------------------------------------------------------------------------------------\n");
            }
        }
    }
};
#endif