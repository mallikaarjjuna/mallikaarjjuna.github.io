#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <strings.h> 
#include "swephexp.h"

/* --- INTERNAL CITY DATABASE --- */
typedef struct {
    char name[50];
    double lat;
    double lon;
    double tz_offset;
} City;

City city_db[] = {
    {"Nellore", 14.450000, 79.986944, 5.5},
    {"Hyderabad", 17.385044, 78.486671, 5.5},
    {"Chennai", 13.082680, 80.270718, 5.5},
    {"NewYork", 40.712776, -74.005974, -5.0},
    {"London", 51.507351, -0.127758, 0.0}
};
int num_cities = sizeof(city_db) / sizeof(city_db[0]);

/* --- ASTROLOGY CONSTANTS --- */
const char *rashi_names[] = {
    "Mesh", "Vrish", "Mitu", "Kark", "Simh", "Kany", 
    "Tula", "Vrishch", "Dhan", "Maka", "Kumb", "Meen"
};

const char *nak_names[] = {
    "Ashwini", "Bharani", "Krittika", "Rohini", "Mrigashira", "Ardra", 
    "Punarvasu", "Pushya", "Ashlesha", "Magha", "Purva Phalguni", 
    "Uttara Phalguni", "Hasta", "Chitra", "Swati", "Vishakha", 
    "Anuradha", "Jyeshtha", "Mula", "Purva Ashadha", "Uttara Ashadha", 
    "Shravana", "Dhanishtha", "Shatabhisha", "Purva Bhadrapada", 
    "Uttara Bhadrapada", "Revati"
};

const char *dasha_lords[] = {
    "Ketu", "Shukra", "Surya", "Chandra", "Mangal", "Rahu", "Guru", "Shani", "Budha"
};

const double dasha_years[] = {
    7.0, 20.0, 6.0, 10.0, 7.0, 18.0, 16.0, 19.0, 17.0
};

/* --- FUNCTIONS --- */
void print_dms(char* planet_name, double decimal_degrees) {
    int rashi_index = (int)(decimal_degrees / 30.0);
    double rashi_degrees = decimal_degrees - (rashi_index * 30.0);
    
    int degrees = (int)rashi_degrees;
    double fractional_deg = rashi_degrees - degrees;
    
    double total_minutes = fractional_deg * 60.0;
    int minutes = (int)total_minutes;
    double fractional_min = total_minutes - minutes;
    
    int seconds = (int)round(fractional_min * 60.0);
    if (seconds >= 60) {
        seconds -= 60;
        minutes += 1;
    }
    if (minutes >= 60) {
        minutes -= 60;
        degrees += 1;
    }
    if (degrees >= 30) {
        degrees -= 30;
        rashi_index = (rashi_index + 1) % 12;
    }

    int navamsha_index = (int)(decimal_degrees / (10.0 / 3.0));
    int d9_rashi_index = navamsha_index % 12;

    printf("%-10s %02d° %-5s %02d' %02d\"  |  D9 Rashi: %-5s\n", 
           planet_name, degrees, rashi_names[rashi_index], minutes, seconds, rashi_names[d9_rashi_index]);
}

void calculate_dasha_balance(double moon_longitude) {
    double nak_size = 360.0 / 27.0; 
    int nak_index = (int)(moon_longitude / nak_size);
    
    double passed_nak_degrees = moon_longitude - (nak_index * nak_size);
    int pada = (int)(passed_nak_degrees / (nak_size / 4.0)) + 1;
    int lord_index = nak_index % 9;
    
    double fraction_passed = passed_nak_degrees / nak_size;
    double fraction_remaining = 1.0 - fraction_passed;
    double total_years_left = fraction_remaining * dasha_years[lord_index];
    
    int y_left = (int)total_years_left;
    double remaining_months = (total_years_left - y_left) * 12.0;
    int m_left = (int)remaining_months;
    double remaining_days = (remaining_months - m_left) * (365.2425 / 12.0); 
    int d_left = (int)round(remaining_days);

    printf("\n=== VIMSHOTTARI DASHA BALANCE ===\n");
    printf("Nakshatra : %s (Pada %d)\n", nak_names[nak_index], pada);
    printf("Maha Dasha: %s\n", dasha_lords[lord_index]);
    printf("Balance   : %d Years, %d Months, %d Days\n", y_left, m_left, d_left);
}

int main(int argc, char *argv[]) {
    /* Updated to expect 8 arguments (including program name) */
    if (argc != 8) {
        printf("Usage: ./astroc <YYYY> <MM> <DD> <HH> <MIN> <SEC> <CityName>\n");
        printf("Example: ./astroc 1981 7 29 13 1 30 Nellore\n");
        return 1;
    }

    int year = atoi(argv[1]);
    int month = atoi(argv[2]);
    int day = atoi(argv[3]);
    int hour = atoi(argv[4]);
    int minute = atoi(argv[5]);
    int second = atoi(argv[6]); /* New seconds parameter */
    char *target_city = argv[7];

    double geo_lat = 0, geo_lon = 0, tz_offset = 0;
    int city_found = 0;

    for (int i = 0; i < num_cities; i++) {
        if (strcasecmp(target_city, city_db[i].name) == 0) {
            geo_lat = city_db[i].lat;
            geo_lon = city_db[i].lon;
            tz_offset = city_db[i].tz_offset;
            city_found = 1;
            break;
        }
    }

    if (!city_found) {
        printf("Error: City '%s' not found in internal database.\n", target_city);
        return 1;
    }

    /* Incorporate seconds into the decimal time calculation */
    double local_time_decimal = hour + (minute / 60.0) + (second / 3600.0);
    double time_ut = local_time_decimal - tz_offset;

    swe_set_ephe_path("./ephe"); 
    swe_set_sid_mode(SE_SIDM_LAHIRI, 0, 0);

    double tjd_ut = swe_julday(year, month, day, time_ut, SE_GREG_CAL);
    swe_set_topo(geo_lon, geo_lat, 0.0);

    int32 iflag = SEFLG_SWIEPH | SEFLG_SIDEREAL | SEFLG_SPEED | SEFLG_TRUEPOS;
    double xx[6]; 
    char serr[256]; 
    double moon_lon_total = 0.0; 

    printf("\n=== PURE ASTRONOMICAL ENGINE OUTPUT ===\n");
    /* Output formatted to show HH:MM:SS */
    printf("Local Date: %02d/%02d/%04d | Local Time: %02d:%02d:%02d\n", day, month, year, hour, minute, second);
    printf("Location: %s (Lat: %f, Lon: %f, TZ: %+.1f)\n", target_city, geo_lat, geo_lon, tz_offset);
    printf("Calculated UT: %.6f\n", time_ut);
    printf("Model: Geometric Positions | True Ayanamsha | True Nodes\n");
    printf("------------------------------------------------------------------\n");

    double cusps[13], ascmc[10];
    if (swe_houses_ex(tjd_ut, iflag, geo_lat, geo_lon, 'P', cusps, ascmc) >= 0) {
        print_dms("Lagna", ascmc[0]);
    }

    int planets[] = {SE_SUN, SE_MOON, SE_MARS, SE_MERCURY, SE_JUPITER, SE_VENUS, SE_SATURN, SE_TRUE_NODE};
    char* names[] = {"Surya", "Chandra", "Mangal", "Budha", "Guru", "Shukra", "Shani", "Rahu"};

    for (int i = 0; i < 8; i++) {
        if (swe_calc_ut(tjd_ut, planets[i], iflag, xx, serr) >= 0) {
            print_dms(names[i], xx[0]);
            if (planets[i] == SE_MOON) {
                moon_lon_total = xx[0]; 
            }
        }
    }

    swe_calc_ut(tjd_ut, SE_TRUE_NODE, iflag, xx, serr);
    double ketu_pos = fmod(xx[0] + 180.0, 360.0);
    print_dms("Ketu", ketu_pos);
    printf("------------------------------------------------------------------\n");

    calculate_dasha_balance(moon_lon_total);

    swe_close();
    return 0;
}