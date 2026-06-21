#pragma once
#include <string>

// =========================================================================
// JYOTISHA ENGINE: EXTERNALIZED VOCABULARY & STRINGS
// =========================================================================

const char* const rashi_names[] = {"Mesh", "Vrish", "Mitu", "Kark", "Simh", "Kany", "Tula", "Vrishch", "Dhan", "Maka", "Kumb", "Meen"};
const char* const short_rashi[] = {"Mes", "Vri", "Mit", "Kar", "Sim", "Kan", "Tul", "Vrc", "Dha", "Mak", "Kum", "Mee"};
const char* const rashi_lords[] = {"Mangal", "Shukra", "Budha", "Chandra", "Surya", "Budha", "Shukra", "Mangal", "Guru", "Shani", "Shani", "Guru"};
const char* const weekdays[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

const char* const nak_names[] = {
    "Ashwini", "Bharani", "Krittika", "Rohini", "Mrigashira", "Ardra", "Punarvasu", "Pushya", "Ashlesha", "Magha", "Purva Phalguni", 
    "Uttara Phalguni", "Hasta", "Chitra", "Swati", "Vishakha", "Anuradha", "Jyeshtha", "Mula", "Purva Ashadha", "Uttara Ashadha", 
    "Shravana", "Dhanishtha", "Shatabhisha", "Purva Bhadrapada", "Uttara Bhadrapada", "Revati"
};

const char* const tara_names[] = {
    "1. Janma (Birth)", "2. Sampat (Wealth)", "3. Vipat (Danger)", "4. Kshema (Well-being)",
    "5. Pratyak (Obstacle)", "6. Sadhaka (Achievement)", "7. Vadha (Destruction)", "8. Mitra (Friend)", "9. Ati-Mitra (Best Friend)"
};

const char* const dasha_lords[] = {"Ketu", "Shukra", "Surya", "Chandra", "Mangal", "Rahu", "Guru", "Shani", "Budha"};
const char* const short_dasha[] = {"Ke", "Ve", "Su", "Mo", "Ma", "Ra", "Ju", "Sa", "Me"};
const double dasha_years[] = {7.0, 20.0, 6.0, 10.0, 7.0, 18.0, 16.0, 19.0, 17.0};
const char* const dasha_levels[] = {"Maha Dasha", "Antar Dasha (Bhukti)", "Pratyantar Dasha", "Sookshma Dasha", "Prana Dasha", "Deha Dasha"};
const char* const p_names_full[] = {"Lagna", "Surya", "Chandra", "Mangal", "Budha", "Guru", "Shukra", "Shani", "Rahu", "Ketu"};

const char* const tithi_names[] = {
    "Pratipada", "Dwitiya", "Tritiya", "Chaturthi", "Panchami", "Shashthi", "Saptami", "Ashtami", "Navami", "Dashami",
    "Ekadashi", "Dwadashi", "Trayodashi", "Chaturdashi", "Purnima", 
    "Pratipada", "Dwitiya", "Tritiya", "Chaturthi", "Panchami", "Shashthi", "Saptami", "Ashtami", "Navami", "Dashami",
    "Ekadashi", "Dwadashi", "Trayodashi", "Chaturdashi", "Amavasya" 
};

const char* const yoga_names[] = {
    "Vishkambha", "Priti", "Ayushman", "Saubhagya", "Shobhana", "Atiganda", "Sukarma", "Dhriti", "Shula", "Ganda",
    "Vriddhi", "Dhruva", "Vyaghata", "Harshana", "Vajra", "Siddhi", "Vyatipata", "Variyan", "Parigha", "Shiva",
    "Siddha", "Sadhya", "Shubha", "Shukla", "Brahma", "Indra", "Vaidhriti"
};

// =========================================================================
// AI TEXT GENERATION MODULES & MATRICES
// =========================================================================

inline std::string get_varga_theme(int v_num) {
    switch(v_num) {
        case 2: return "Hora (D2): The micro-environment governing accumulated wealth, liquid assets, and the financial sustenance of the native.";
        case 3: return "Drekkana (D3): The matrix of courage, initiative, co-borns (siblings), and the capacity to overcome micro-traumas and immediate obstacles.";
        case 4: return "Chaturthamsha (D4): The foundation chart. It reveals fixed properties, real estate, emotional rootedness, and overall destiny regarding one's physical home.";
        case 7: return "Saptamsha (D7): The creative legacy chart. It dictates the outcomes regarding progeny, children, and the karmic legacy left behind.";
        case 9: return "Navamsha (D9): The most critical harmonic. It maps the internal soul, the reality of partnerships (marriage), and the true trajectory of the second half of life.";
        case 10: return "Dashamsha (D10): The battlefield of public life. It maps career trajectory, karmic achievements, and interactions with authority and society at large.";
        case 11: return "Rudramsha (D11): The chart of ultimate gains, elder siblings, and the destructive (Rudra) forces required to break through stagnation.";
        case 12: return "Dwadashamsha (D12): The ancestral blueprint. It reveals the karma inherited from parents, genetic lineage, and early childhood conditioning.";
        case 16: return "Shodashamsha (D16): The realm of vehicles, profound inner luxuries, and the psychological capacity to actually enjoy what one has acquired.";
        case 20: return "Vimshamsha (D20): The spiritual compass. It charts devotion, religious inclinations, and the evolution of the soul's relationship with the divine.";
        case 24: return "Chaturvimshamsha (D24): The cognitive matrix. It maps intellect, educational achievements, and the assimilation of higher knowledge.";
        case 27: return "Bhamsha (D27): The spectrum of intrinsic physical and mental endurance, mapping underlying strengths and hidden vulnerabilities.";
        case 30: return "Trimshamsha (D30): The shadow chart. It reveals deep-seated subconscious fears, karmic misfortunes, miseries, and latent diseases.";
        case 40: return "Khavedamsha (D40): A highly granular mapping of the auspicious and inauspicious effects of overarching macro-karma.";
        case 45: return "Akshavedamsha (D45): The harmonic of general moral conduct, ethical frameworks, and the character of the soul's decisions.";
        case 60: return "Shashtiamsha (D60): The master fine-tuning chart. It holds the dense, specific residue of past-life karma dictating exact life events.";
        default: return "Specific Micro-Area of Life.";
    }
}

inline std::string get_varga_engagement(int h) {
    if (h == 1) return "Core Engagement: Placed in the Ascendant. This energy directly dictates the literal, physical manifestation and core identity of this specific area of life.";
    if (h == 2 || h == 11) return "Sustaining Engagement: Placed in a house of growth. This acts as a sustainer, providing resources, gains, and compounding growth to this specific theme.";
    if (h == 3 || h == 6) return "Friction Engagement: Placed in an Upachaya/Combat house. This placement forces you to fight, exert immense courage, and defeat obstacles regarding this theme. It is highly competitive.";
    if (h == 4 || h == 7 || h == 10) return "Power Engagement: Placed in a Kendra. This demands strong, visible, and immediate action. This planet is a primary actor and powerful driving force here.";
    if (h == 5 || h == 9) return "Grace Engagement: Placed in a Trikona. This indicates natural grace, past-life blessings, and an effortless ease regarding the themes of this chart.";
    if (h == 8) return "Transformative Engagement: Placed in the 8th Dusthana. This warns of sudden breaks, chronic issues, or deep psychological trauma—but also profound hidden transformations related to this theme.";
    if (h == 12) return "Detachment Engagement: Placed in the 12th Dusthana. This strongly indicates literal loss, required isolation, a connection to foreign elements, or heavy expenditures regarding this theme.";
    return "Neutral Engagement: Operates dynamically based on conjunctions and aspects.";
}

inline std::string get_digbala_text(int p_idx, int h) {
    if (p_idx == 3 && h == 10) return " * Maximum Digbala: Mars acquires profound directional strength here, granting immense execution capabilities and a commanding, forceful presence in career.";
    if (p_idx == 1 && h == 10) return " * Maximum Digbala: The Sun acquires profound directional strength here, establishing natural authority, leadership, and highly visible public status.";
    if (p_idx == 4 && h == 1)  return " * Maximum Digbala: Mercury acquires profound directional strength here, creating a highly adaptable, sharply intellectual, and communicative core nature.";
    if (p_idx == 5 && h == 1)  return " * Maximum Digbala: Jupiter acquires profound directional strength here, wrapping the native in an aura of wisdom, grace, and natural philosophical guidance.";
    if (p_idx == 6 && h == 4)  return " * Maximum Digbala: Venus acquires profound directional strength here, amplifying emotional comforts, artistic sensibilities, and a deep appreciation for domestic luxury.";
    if (p_idx == 2 && h == 4)  return " * Maximum Digbala: The Moon acquires profound directional strength here, creating a vast capacity for empathy, emotional intelligence, and public connectivity.";
    if (p_idx == 7 && h == 7)  return " * Maximum Digbala: Saturn acquires profound directional strength here, granting the stamina to build lasting public networks, endure partnership trials, and achieve mass influence.";
    return "";
}

inline std::string get_final_outcome(int p_idx, bool is_ben, bool is_mal, bool is_ken, bool is_dust, bool is_kt, int h, std::string disp_name) {
    if (p_idx == 8 || p_idx == 9) { 
        if (h==3 || h==6 || h==10 || h==11) return "Highly Auspicious (Upachaya Node). Placed in a house of growth, this shadow planet will grow exceptionally strong over time, granting sudden and massive material success by aggressively channeling the energy of its dispositor, " + disp_name + ".";
        else if (is_dust) return "Highly Volatile (Dusthana Node). Placed in an unstable house, this shadow planet amplifies hidden fears, sudden expenses, or psychological volatility, operating entirely through the disrupted energy of " + disp_name + ".";
        else return "Karmic Amplifier. Placed in a core angular or trine house, this shadow planet heavily eclipses the standard energy here, amplifying its own karmic themes while relying entirely on the foundational strength of " + disp_name + " to deliver physical results.";
    } 
    if (is_mal && is_dust) return "Vipareeta Raja Yoga (Mixed but Ultimately Victorious). Because a challenging lord is placed in a challenging house, the negatives mathematically cancel out. Expect significant early struggles, enemies, or debts that eventually reverse to trigger your greatest rise and success.";
    if (is_ben && is_ken && is_dust) return "Delayed Grace (Auspicious but Slow). A highly supportive planet is placed in a house of loss or obstacles. Ultimate success will manifest, but only after intense patience, grinding effort, or physically relocating away from your place of birth.";
    if (is_ben && is_kt) return "Highly Auspicious (Excellent). Operating with excellent lordship and placed in a house of power or grace, this planet serves as a major, reliable pillar of support, bringing continuous fortune, intellect, and ease to your life.";
    if (is_mal && is_kt) return "Highly Volatile (Powerful but High Stress). This planet brings immense drive, extreme potential for authority, and raw power, but carries a difficult energetic job description. Expect sudden fluctuations, intense stress, or the need to aggressively enforce boundaries.";
    if (is_dust) return "Challenging (Karmic Focus). Confined to an obstacle house, this planet demands deep, uncomfortable transformation. It requires you to navigate hidden traumas and forces areas of literal loss to teach necessary detachment.";
    if (h==3 || h==6 || h==10 || h==11) return "Growth Oriented (Demanding but Rewarding). Operating from an Upachaya (growing) house, this planet provides steady gains and aids in building networks, but it will force you to work relentlessly and occasionally sacrifice peace to achieve your ambitions.";
    if (is_kt) return "Supportive and Stabilizing (Neutral). Placed in a foundational house, this planet acts as a steady stabilizing force, bringing natural grace and unforced support to the native's life trajectory.";
    return "Neutral House Placement. Operates dynamically based on conjunctions and transits.";
}

inline int get_varga_karaka(int v_num) {
    switch(v_num) {
        case 2: return 5; case 3: return 3; case 4: return 2; case 7: return 5;
        case 9: return 6; case 10: return 4; case 12: return 1; case 16: return 6;
        case 20: return 5; case 24: return 4; case 27: return 3; case 30: return 7;
        default: return -1;
    }
}

inline std::string get_vargottama_text(std::string p_name) { return p_name + " is VARGOTTAMA. By occupying the exact same sign in both the D1 and this micro-chart, its energy is permanently crystallized. This planet acts with supreme confidence, structural rigidity, and massive fated influence in this area of life."; }

inline std::string get_d1_lord_in_varga_text(std::string d1_lord_name, int h) {
    if (h == 1 || h == 5 || h == 9) return "Highly Auspicious Alignment. Placed in a Grace/Dharma house (House " + std::to_string(h) + ") in this micro-chart, it shows your physical self naturally aligns with and thrives in this area of life.";
    if (h == 4 || h == 7 || h == 10) return "Heavy Action Demanded. Placed in an Angle/Kendra (House " + std::to_string(h) + ") in this micro-chart, it forces you to take heavy, direct, and highly visible action regarding these themes.";
    if (h == 6 || h == 8 || h == 12) return "Karmic Friction and Exhaustion. Placed in a Dusthana (House " + std::to_string(h) + ") in this micro-chart, it indicates that engaging with these themes will physically or mentally exhaust you, requiring deep transformation or literal loss.";
    return "Persistent Growth Required. Placed in an Upachaya house (House " + std::to_string(h) + "), your mastery over this area of life will start slow but grow massively through aggressive, persistent effort.";
}

// MASSIVE 108-ITEM 2D KNOWLEDGE MATRIX (9 Planets x 12 Houses)
const char* const planet_in_house_matrix[10][13] = {
    {""},
    {"", "The Sun in the 1st House creates a highly independent, radiant, and authoritative personality. The native naturally seeks leadership but must guard against ego clashes and physical heat/burnout.", "The Sun in the 2nd House illuminates wealth and family lineage. It grants an authoritative tone of speech and gains through government or authority figures, though it can cause friction in close family matters.", "The Sun in the 3rd House is extremely powerful, granting immense courage, willpower, and the absolute ability to crush competitors. However, it often creates ideological friction with younger siblings.", "The Sun in the 4th House brings a deep, private desire for internal power and control over the home environment. It can cause a restless domestic life and frequent emotional anxieties.", "The Sun in the 5th House creates a brilliantly sharp intellect and a flair for politics or speculation. It indicates high intelligence but often causes delays or ego-friction regarding children.", "The Sun in the 6th House is a conqueror placement. The native effortlessly defeats enemies, excels in litigation, and often succeeds in medical, competitive, or service-oriented fields.", "The Sun in the 7th House projects authority into partnerships. The native often attracts a dominant or high-status spouse, leading to potential power struggles and ego clashes in marriage or business.", "The Sun in the 8th House places the soul in the realm of the hidden. It grants profound research abilities and an interest in the occult, but warns of sudden transformations and issues with government/taxes.", "The Sun in the 9th House indicates a deeply philosophical, righteous, and ideological nature. The native holds high ideals, often leading to clashes with the father or established traditional gurus.", "The Sun in the 10th House gains maximum directional strength. This is a supreme placement for executive power, public visibility, massive career success, and seamless interaction with government or authorities.", "The Sun in the 11th House guarantees immense material gains through large networks, influential friends, and authority figures. It makes the native highly ambitious and successful in achieving their desires.", "The Sun in the 12th House separates the ego from the material world. It indicates success in foreign lands, hidden or isolated career paths (like hospitals or labs), and a deep, underlying spiritual quest."},
    {"", "The Moon in the 1st House creates a magnetic, empathetic, and highly adaptable personality. The native's mind and physical body are deeply intertwined, making them highly responsive to their environment.", "The Moon in the 2nd House creates a soft, persuasive speaker whose financial state fluctuates with their emotional state. Wealth often comes through public-facing roles, food, or liquid assets.", "The Moon in the 3rd House creates a highly communicative and restless mind. The native loves short travels, writing, and networking, maintaining deep emotional bonds with siblings and neighbors.", "The Moon in the 4th House gains maximum directional strength. It grants profound internal peace, deep attachment to the mother and homeland, and a natural affinity for real estate and domestic comfort.", "The Moon in the 5th House blesses the native with an intensely creative and romantic mind. It indicates strong emotional intelligence, a love for the arts, and deep affection for children.", "The Moon in the 6th House places the emotional mind in a battlefield. The native is prone to anxiety and fluctuating health, but excels in service-oriented roles, healing, and resolving disputes.", "The Moon in the 7th House seeks deep emotional fulfillment through marriage and business partnerships. The native often marries young or attracts a sensitive, public-facing, or emotionally volatile spouse.", "The Moon in the 8th House forces the mind to navigate hidden depths. It grants intense intuition and psychological insight, but makes the native prone to sudden emotional turbulence and hidden fears.", "The Moon in the 9th House is highly auspicious, blessing the native with a naturally graceful, philosophical, and optimistic mind. It indicates deep faith, long travels, and profound support from the father/gurus.", "The Moon in the 10th House connects the emotional mind to public life. Career trajectory may be highly fluctuating or involve constant interaction with the masses, public relations, healing, or travel.", "The Moon in the 11th House grants a vast network of friends and highly fruitful social connections. The native achieves their desires smoothly and maintains a constantly expanding social circle.", "The Moon in the 12th House creates a deeply imaginative, private, and spiritually inclined mind. The native requires frequent isolation to recharge and often finds emotional peace in foreign lands or ashrams."},
    {"", "Mars in the 1st House creates a fiercely independent, highly energetic, and physically robust native. It grants an aggressive, pioneering spirit but can make the native prone to impulsiveness and head injuries.", "Mars in the 2nd House brings intense drive towards wealth accumulation, often through technical or real estate means. It creates a harsh, direct, and argumentative tone of speech.", "Mars in the 3rd House is extremely powerful. It grants unstoppable willpower, athletic prowess, and a fearless nature. The native crushes obstacles but may fiercely dominate their younger siblings.", "Mars in the 4th House disrupts domestic peace with aggressive or controlling energy. It creates a strong drive to acquire real estate and properties, but indicates friction with the mother and internal unrest.", "Mars in the 5th House creates a brilliant, deeply analytical, and competitive intellect. The native loves strategy and sports, but may experience aggressive friction in romances or delays regarding children.", "Mars in the 6th House is a flawless placement for destroying enemies. The native is a natural fighter, excelling in litigation, surgery, or competitive environments, completely overpowering their opposition.", "Mars in the 7th House injects highly volatile and aggressive energy into partnerships. It creates dominant business alliances and extreme passion, but warns of severe friction and arguments in marriage.", "Mars in the 8th House creates a fearless approach to the unknown. It grants deep research abilities and intense transformative energy, but warns of sudden accidents, surgeries, or unexpected upheavals.", "Mars in the 9th House makes the native an aggressive defender of their own ideology or religion. They fiercely debate philosophies and possess a highly independent, self-made approach to their dharma.", "Mars in the 10th House gains maximum directional strength. This creates an unstoppable, highly ambitious executive. The native dominates their career field with ruthless efficiency, technical skill, and commanding authority.", "Mars in the 11th House is highly auspicious for wealth. It grants massive, aggressively acquired gains, highly ambitious goals, and a commanding presence within large social or corporate networks.", "Mars in the 12th House directs aggressive energy inward or toward foreign matters. It indicates heavy expenditures, hidden enemies, restless sleep, and a tendency to fight unseen battles."},
    {"", "Mercury in the 1st House gains maximum directional strength. It creates an exceptionally youthful, highly adaptable, and brilliant native who thrives on data, logic, and rapid communication.", "Mercury in the 2nd House creates a flawless, persuasive, and often highly humorous speaker. Wealth is accumulated through business acumen, consulting, writing, or intellectual pursuits.", "Mercury in the 3rd House feels right at home, granting phenomenal writing, marketing, and networking skills. The native has a highly active, curious mind and excellent relations with siblings.", "Mercury in the 4th House brings constant intellectual activity into the home environment. The native loves studying, home-based businesses, and maintains a highly communicative relationship with the mother.", "Mercury in the 5th House indicates profound academic intelligence and a rapid learning curve. The native excels in mathematics, programming, or speculative trading, and enjoys mentally stimulating romances.", "Mercury in the 6th House creates a razor-sharp, highly critical problem solver. The native excels at organizing chaos, winning disputes through logic, and analyzing data, though it can cause nervous anxiety.", "Mercury in the 7th House creates a strong desire for a highly intellectual, communicative, and youthful partner. Marriage is often treated like a logical partnership or friendly business alliance.", "Mercury in the 8th House grants a brilliant investigative mind. The native excels at uncovering secrets, deep research, astrology, and navigating complex financial structures or taxes.", "Mercury in the 9th House links logic to higher philosophy. The native loves higher education, long-distance travel, publishing, and analyzes religion or dharma through a highly intellectual, logical lens.", "Mercury in the 10th House creates a highly visible intellectual career. The native excels in commerce, administration, IT, or communications, smoothly navigating corporate ladders through strategic networking.", "Mercury in the 11th House is an excellent placement for business gains. The native maintains a massive network of acquaintances and achieves immense financial success through trade, logic, and communication.", "Mercury in the 12th House creates a highly active subconscious mind. The native excels at hidden research, foreign languages, or coding, but may struggle with overthinking or communicating their deepest thoughts."},
    {"", "Jupiter in the 1st House gains maximum directional strength, wrapping the native in an aura of grace, optimism, and profound wisdom. It protects the physical body and ensures a highly respected, noble life path.", "Jupiter in the 2nd House blesses the native with immense wealth, an expanding family lineage, and a highly truthful, philosophical tone of speech. Money flows easily through advisory or educational roles.", "Jupiter in the 3rd House expands the native's courage through wisdom. They are highly optimistic, excellent teachers or writers, and maintain a deeply supportive relationship with their siblings and peers.", "Jupiter in the 4th House brings massive blessings, peace, and expansion to the domestic life. The native enjoys large homes, a deeply spiritual or supportive mother, and profound internal contentment.", "Jupiter in the 5th House is the ultimate placement for higher intelligence, divine grace, and exceptional progeny. It indicates past-life good karma, profound advisory skills, and natural luck in speculation.", "Jupiter in the 6th House protects the native from enemies through wisdom and law rather than combat. It grants success in service, healing, or legal professions, though it can expand debts if not careful.", "Jupiter in the 7th House blesses the native with a highly noble, wise, and supportive spouse. It expands business networks naturally and ensures highly ethical, prosperous partnerships.", "Jupiter in the 8th House grants profound occult and psychological wisdom. It protects the native during sudden transformations, grants hidden inheritances, and creates a highly intuitive, secretive researcher.", "Jupiter in the 9th House is the pure embodiment of Dharma. It creates a deeply religious, highly educated native who naturally attracts excellent gurus, long-distance travel, and divine fortune.", "Jupiter in the 10th House creates a highly respected, ethical, and advisory career. The native easily attains positions of management, teaching, or legal authority, universally respected by society.", "Jupiter in the 11th House guarantees immense, expanding wealth and a highly influential network of powerful friends. The native effortlessly achieves their highest ambitions through grace and networking.", "Jupiter in the 12th House expands the native's spiritual dimensions. It grants deep peaceful sleep, successful foreign travel, massive success in ashrams or isolated work, and acts as a pure guardian angel."},
    {"", "Venus in the 1st House creates an exceptionally beautiful, magnetic, and charismatic personality. The native possesses highly refined tastes, a love for luxury, and naturally attracts harmony and attention.", "Venus in the 2nd House creates a sweet, highly charming speaker who easily accumulates liquid wealth and luxury assets. The native enjoys fine foods, beautiful family environments, and artistic financial pursuits.", "Venus in the 3rd House brings grace and diplomacy to communication. The native excels in artistic hobbies, creative writing, and maintains highly affectionate, harmonious relationships with their siblings.", "Venus in the 4th House gains maximum directional strength, bringing profound luxury, vehicles, and beauty into the home. It ensures deep inner happiness, a loving mother, and a highly aesthetic domestic life.", "Venus in the 5th House creates a deeply romantic, highly creative, and artistic native. It indicates a love for entertainment, successful speculative investments in luxury, and beautiful, affectionate children.", "Venus in the 6th House forces the planet of harmony into the house of conflict. The native often works in fields related to healing, women, or resolving disputes, but must guard against health issues from overindulgence.", "Venus in the 7th House creates a profound desire for a beautiful, harmonious marriage. The native attracts a highly aesthetic or wealthy partner and excels effortlessly in public relations and business alliances.", "Venus in the 8th House grants a love for the mysterious and hidden. It indicates sudden financial gains through partners or inheritance, deep, intense secret romances, and a highly transformative life path.", "Venus in the 9th House blesses the native with a love for foreign cultures, higher philosophies, and long-distance travel. They experience natural grace, artistic higher education, and an effortless, lucky dharma.", "Venus in the 10th House creates a highly visible career involving arts, luxury, women, or diplomacy. The native is widely loved in their professional circle and easily attains status through charm and creativity.", "Venus in the 11th House is an ultimate wealth placement. It guarantees massive financial gains, a vast network of female or artistic friends, and the effortless fulfillment of all material and romantic desires.", "Venus in the 12th House is a unique placement where Venus thrives in the house of bed pleasures. It grants profound physical luxuries, success in foreign lands, and deep, imaginative, spiritual artistic abilities."},
    {"", "Saturn in the 1st House creates a serious, highly disciplined, and mature personality from a young age. The native is hardworking and realistic, but must guard against melancholy, self-doubt, or physical delays.", "Saturn in the 2nd House makes the native highly conservative with wealth and cautious in speech. Financial stability comes slowly but permanently through immense grinding effort and strict discipline.", "Saturn in the 3rd House is extremely powerful, granting unbreakable willpower, monumental patience, and the stamina to outlast any competitor. The native communicates seriously and dominates through endurance.", "Saturn in the 4th House forces early maturity and creates an emotionally detached or highly structured home environment. It grants lasting real estate later in life but requires overcoming deep internal anxieties.", "Saturn in the 5th House slows down the results of education, romance, and progeny, demanding extreme structure. The native possesses a deep, serious intellect suited for engineering, history, or hard sciences.", "Saturn in the 6th House is a phenomenal placement. The native slowly, methodically, and permanently crushes their enemies and diseases. It creates an unstoppable work ethic and massive success in service or legal fields.", "Saturn in the 7th House gains maximum directional strength. It delays marriage to ensure absolute stability, attracting an older, mature, or hardworking partner, and grants the stamina to build massive public networks.", "Saturn in the 8th House significantly increases the native's physical longevity. It creates a deeply serious, secretive researcher who must navigate heavy karmic transformations and structured occult knowledge.", "Saturn in the 9th House indicates a deeply traditional, rigid, or highly disciplined approach to religion and philosophy. It can cause early delays or friction with the father, forcing the native to build their own belief system.", "Saturn in the 10th House creates the ultimate slow-burning executive. Career rise is slow, demanding, and requires monumental effort, but leads to permanent, unshakable authority and massive public status.", "Saturn in the 11th House is highly auspicious, systematically building massive networks and permanent wealth over time. The native's desires and long-term goals are achieved with absolute certainty through delayed gratification.", "Saturn in the 12th House forces the native to confront isolation, loss, and hidden debts. It creates an incredibly disciplined spiritual seeker and often indicates massive, structured success in completely foreign lands or isolated environments."},
    {"", "Rahu in the 1st House creates an intensely ambitious, unconventional, and highly charismatic personality. The native is obsessed with self-development and breaking boundaries, projecting a highly magnetic, illusionary aura.", "Rahu in the 2nd House creates an obsessive desire for massive wealth accumulation and unconventional assets. The native possesses a highly persuasive, sometimes manipulative voice, and must guard against unorthodox financial risks.", "Rahu in the 3rd House is a phenomenal placement. It grants explosive courage, extreme competitive drive, and brilliant, out-of-the-box communication skills. The native crushes all opposition through sheer ambition.", "Rahu in the 4th House creates a restless, unconventional home environment and a deep inner craving for massive properties. The native often relocates far from their birthplace or experiences intense emotional fluctuations.", "Rahu in the 5th House amplifies a brilliant, unorthodox, and highly speculative intellect. The native excels in modern technology, politics, or media, but experiences intense, unusual romances and potential illusions regarding children.", "Rahu in the 6th House is a flawless placement for material dominance. The native effortlessly destroys enemies, masters foreign or technical subjects, and completely overpowers any competitive or legal opposition.", "Rahu in the 7th House creates an intense obsession with partnerships and the public. The native often attracts foreign, unconventional, or highly ambitious spouses, leading to extreme highs and lows in marriage and business.", "Rahu in the 8th House plunges the native into the extremes of the occult, hidden wealth, and deep psychological research. It warns of sudden, massive transformations, unconventional secrets, and sudden financial windfalls or losses.", "Rahu in the 9th House creates a highly unorthodox, boundary-breaking approach to religion and philosophy. The native frequently travels long distances, studies foreign cultures, and builds their own unique, non-traditional dharma.", "Rahu in the 10th House is an explosive career placement. The native is obsessed with public status and achieves massive, sudden authority, frequently involving foreign elements, technology, politics, or highly visible mass manipulation.", "Rahu in the 11th House is the ultimate placement for sudden, massive material gains. The native builds a vast, powerful, and unconventional network of friends, effortlessly and aggressively manifesting their highest worldly ambitions.", "Rahu in the 12th House creates a highly active, imaginative, and sometimes chaotic subconscious mind. The native experiences extreme foreign travels, deep spiritual illusions, and intense, vivid dreams or hidden expenses."},
    {"", "Ketu in the 1st House creates a deeply introverted, spiritual, and highly intuitive personality. The native feels detached from their physical body and worldly ego, possessing profound, unexplainable past-life insights.", "Ketu in the 2nd House creates a natural detachment from material wealth and family lineage. Money often comes and goes without the native's active effort, and their speech is often brutally blunt, philosophical, or highly cryptic.", "Ketu in the 3rd House grants sudden bursts of courage and a highly intuitive, sharp communication style. The native is detached from sibling politics and easily cuts through immediate obstacles with minimal effort.", "Ketu in the 4th House creates a profound feeling of detachment from the birthplace, mother, or physical home. The native seeks deep internal moksha and often feels like a stranger in their own domestic environment.", "Ketu in the 5th House brings past-life genius, especially in mathematics, coding, or ancient texts. However, it creates a deep detachment from modern romances, speculative risks, and conventional approaches to raising children.", "Ketu in the 6th House is an excellent placement for spiritual or medical service. The native is immune to the politics of enemies and quietly, almost invisibly, resolves disputes, though diseases can be mysterious and hard to diagnose.", "Ketu in the 7th House creates a deep, karmic detachment from marriage and public partnerships. The native often feels disconnected from their spouse or attracts highly spiritual, unconventional partners, requiring constant boundary setting.", "Ketu in the 8th House is the ultimate placement for deep occult mastery, astrology, and spiritual liberation. The native possesses extreme psychic intuition and a fearless detachment toward sudden transformations and the unknown.", "Ketu in the 9th House creates a naturally pure, deeply spiritual, and highly philosophical native. They possess innate past-life wisdom, often rejecting traditional religious dogmas in favor of absolute, direct spiritual truth.", "Ketu in the 10th House creates a native who is highly skilled but deeply detached from public status or corporate ladders. They often excel in highly specialized, isolated, or spiritual careers, feeling indifferent to worldly authority.", "Ketu in the 11th House creates a detachment from large social networks and typical material ambitions. The native achieves gains effortlessly due to past-life merit, but feels deeply indifferent to worldly status or superficial friendships.", "Ketu in the 12th House is the ultimate placement for Moksha (Spiritual Liberation). The native possesses profound subconscious clarity, requires deep isolation, and is actively burning off their final karmic debts in this lifetime."}
};

inline std::string get_planet_in_house_text(int p_idx, int h) {
    if (p_idx >= 1 && p_idx <= 9 && h >= 1 && h <= 12) {
        return planet_in_house_matrix[p_idx][h];
    }
    return "Placement analysis pending.";
}

const char* const en_bhava_lord_matrix[13][13] = {
    {""},
    {"", "1st Lord in 1st House: Independent, self-made, strong vitality, and natural leadership. You are the architect of your own destiny.", "1st Lord in 2nd House: Focus is on wealth accumulation, family growth, and authoritative speech. You are highly protective of your assets.", "1st Lord in 3rd House: Immense courage, artistic skills, and success through extreme self-effort and networking.", "1st Lord in 4th House: Deep attachment to mother and home; acquires significant real estate and profound inner peace.", "1st Lord in 5th House: Highly intelligent, excellent progeny; strong past-life karma (Poorva Punya) blesses your endeavors.", "1st Lord in 6th House: Conquers enemies, succeeds in medical/service fields, but prone to pushing physical health to the limit.", "1st Lord in 7th House: Life revolves around partnerships; indicates a successful marriage and a highly public-facing career.", "1st Lord in 8th House: Secretive nature; sudden transformations, interest in the occult, and incredibly deep mental resilience.", "1st Lord in 9th House: Highly fortunate, righteous; protected by divine grace, support from father, and long-distance travels.", "1st Lord in 10th House: Extreme career focus; achieves great authority, status, and public recognition through personal effort.", "1st Lord in 11th House: Massive financial gains, large networks, social influence, and easy fulfillment of material desires.", "1st Lord in 12th House: Highly spiritual, charitable; tends to settle abroad or seek peace in isolated environments."},
    {"", "2nd Lord in 1st House: Wealth comes to you naturally. Family and financial security are the core pillars of your personality.", "2nd Lord in 2nd House: Excellent for accumulating massive wealth. Indicates large fixed assets and a strong family heritage.", "2nd Lord in 3rd House: Wealth is generated through courage, communication, media, and self-made ventures.", "2nd Lord in 4th House: Massive financial gains through real estate, agriculture, or maternal inheritances.", "2nd Lord in 5th House: Wealth flows through intellect, speculation, consulting, and the success of your children.", "2nd Lord in 6th House: Income through medical, legal, or service fields, though prone to financial disputes with family.", "2nd Lord in 7th House: Wealth is generated through business partnerships, foreign trade, or directly via the spouse.", "2nd Lord in 8th House: Hidden wealth, sudden inheritances, or intense financial fluctuations and transformations.", "2nd Lord in 9th House: Foreign wealth, luck through higher education, publishing, or blessings from mentors.", "2nd Lord in 10th House: Highest income comes through career status, government connections, and positions of authority.", "2nd Lord in 11th House: Massive wealth through investments, banking, and highly profitable social networks.", "2nd Lord in 12th House: Wealth is drained through heavy expenses, charity, or foreign investments."},
    {"", "3rd Lord in 1st House: Self-made nature; you use physical effort and extreme courage in highly competitive environments.", "3rd Lord in 2nd House: You use your courage and communication skills to build wealth. Siblings may aid in your finances.", "3rd Lord in 3rd House: Immense mental courage, flawless communication; guarantees success in media, sales, or intense competition.", "3rd Lord in 4th House: Property gains through siblings or self-effort, though the home environment is often highly active or restless.", "3rd Lord in 5th House: Sharp, competitive intellect. Success in sports, arts, and technical fields. Highly imaginative.", "3rd Lord in 6th House: Easily crushes enemies; absolute victory in legal or athletic arenas.", "3rd Lord in 7th House: Attracts a courageous partner. Success through travel, but prone to ego clashes in marriage.", "3rd Lord in 8th House: Extreme courage during sudden transformations. Highly secretive communication and mental resilience.", "3rd Lord in 9th House: Fights for ideologies; independent religious views, philosophical debates, and long-distance travel.", "3rd Lord in 10th House: Highly ambitious career; massive success in PR, media, technical, or military fields.", "3rd Lord in 11th House: Bold decisions, relentless networking, and project management lead to massive financial gains.", "3rd Lord in 12th House: Courage in foreign lands or hidden research; prone to hidden enemies or sudden energy drains."},
    {"", "4th Lord in 1st House: Deep love for the mother. Material comforts, vehicles, and properties manifest easily and naturally.", "4th Lord in 2nd House: Massive family wealth achieved through ancestral properties and real estate.", "4th Lord in 3rd House: Real estate gains through self-effort; often settles away from the birthplace.", "4th Lord in 4th House: Unshakeable inner peace, luxurious home environment, vast properties, and maternal blessings.", "4th Lord in 5th House: Happiness through higher education, deep friendship with the mother, and a creative home life.", "4th Lord in 6th House: Property disputes are likely, but guarantees success in real estate litigation or medical/service fields.", "4th Lord in 7th House: Properties acquired through marriage/spouse; successful business in the birthplace or foreign lands.", "4th Lord in 8th House: Sudden inheritances or property loss; hidden issues in the home and deep psychological transformations.", "4th Lord in 9th House: Extreme luck regarding properties; settling abroad; highly religious or philosophical home environment.", "4th Lord in 10th House: Great public image built through real estate, politics, or a career operated from the home.", "4th Lord in 11th House: Multiple properties, vast social circles, and gains through long-term physical assets.", "4th Lord in 12th House: Detachment from the birthplace; permanently settling abroad or seeking peace in isolated ashrams."},
    {"", "5th Lord in 1st House: Extraordinary intellect, highly creative, academically driven, and a magnetic romantic personality.", "5th Lord in 2nd House: Wealth expands rapidly through intelligence, consulting, and speculative investments.", "5th Lord in 3rd House: Intellectual courage; wonderful relationships with siblings and success in media/writing.", "5th Lord in 4th House: Joy through higher education, a strong bond with the mother, and a creative home life.", "5th Lord in 5th House: Brilliant intellect, excellent children, strong past-life karma, and massive speculative luck.", "5th Lord in 6th House: Intellect is used to defeat enemies; success in competitive exams; minor delays regarding children.", "5th Lord in 7th House: Love marriage or a highly intelligent spouse; joint success in creative business ventures.", "5th Lord in 8th House: Interest in secret knowledge (astrology, tantra); unexpected romance and deep research skills.", "5th Lord in 9th House: Spiritual intellect, foreign higher education, publishing, and immense luck.", "5th Lord in 10th House: Great career in consulting, teaching, or creative arts; highly supported by authorities.", "5th Lord in 11th House: Massive financial gains through speculation, stock markets, and creative networks.", "5th Lord in 12th House: Spiritual intellect, foreign education, deep imagination; physical distance from children."},
    {"", "6th Lord in 1st House: Easily conquers illnesses; guarantees immense success in military, medical, or service fields.", "6th Lord in 2nd House: Wealth through medical, legal, or service sectors; potential for family financial disputes.", "6th Lord in 3rd House: Intense competitive nature; defeats enemies easily through courage, logic, and communication.", "6th Lord in 4th House: Domestic disputes; success in real estate litigation or agricultural service sectors.", "6th Lord in 5th House: Highly analytical intellect; stressful/competitive environment regarding children or investments.", "6th Lord in 6th House: HARSHA YOGA. Absolute victory over enemies, debts, and diseases. Great fame in service.", "6th Lord in 7th House: Friction, delays, or legal issues in marriage; spouse may be in medical/legal/defense fields.", "6th Lord in 8th House: VIPAREETA RAJAYOGA. Emerges from massive crises, debts, and enemies into sudden, spectacular success.", "6th Lord in 9th House: Ideological clashes with father/religion; success in foreign legal matters after initial friction.", "6th Lord in 10th House: Career in medicine, law, defense, or HR; utterly defeats enemies in the workplace.", "6th Lord in 11th House: Gains arrive only after overcoming obstacles; financial success through intense effort and large networks.", "6th Lord in 12th House: VIPAREETA RAJAYOGA. Destroys enemies through isolation, hidden strategies, or foreign assistance."},
    {"", "7th Lord in 1st House: The spouse is deeply connected to your personality, physical life, and public image.", "7th Lord in 2nd House: Massive wealth generated through marriage or business partnerships; success in family business.", "7th Lord in 3rd House: Courageous partner; joint success in networking, media, or frequent travels.", "7th Lord in 4th House: Properties acquired through the spouse; domestic peace; excellent business profits in the birthplace.", "7th Lord in 5th House: Love marriage; highly artistic and intelligent partner; joint success in investment ventures.", "7th Lord in 6th House: Friction, separation, or legal issues in marriage; spouse may be in medical/legal fields.", "7th Lord in 7th House: Wonderful marriage, highly successful business partnerships, and immense growth in public image.", "7th Lord in 8th House: Sudden transformations in marriage; hidden wealth or sudden, unearned financial gains through the spouse.", "7th Lord in 9th House: Foreign or cross-cultural spouse; immense luck and foreign travels triggered after marriage.", "7th Lord in 10th House: Massive career success after marriage; sharing authority or business leadership with the spouse.", "7th Lord in 11th House: Huge financial gains through business partnerships; spouse brings a massive, profitable social network.", "7th Lord in 12th House: Foreign spouse, settling abroad after marriage, or engaging in highly secretive business ventures."},
    {"", "8th Lord in 1st House: Sudden physical transformations; a highly mystical, secretive personality with deep mental resilience.", "8th Lord in 2nd House: Sudden fluctuations in wealth; acquisition of deep ancestral secrets or massive inheritances.", "8th Lord in 3rd House: Extreme courage during crises; secretive communication, profound willpower, and research skills.", "8th Lord in 4th House: Sudden changes in residence; uncovering hidden truths about property/mother, or property loss.", "8th Lord in 5th House: Unusual intellect, sudden twists in romance, and a profound grasp of mystical sciences.", "8th Lord in 6th House: VIPAREETA RAJAYOGA. Emerges from massive crises, debts, and enemies into sudden, spectacular success.", "8th Lord in 7th House: Secrets or sudden transformations in partnerships; unexpected twists in marriage or business.", "8th Lord in 8th House: Great longevity, mastery of mystical sciences, and the mental resilience to easily survive any crisis.", "8th Lord in 9th House: Unconventional ideologies, sudden changes in foreign travel, and deep exploration/rejection of orthodox religion.", "8th Lord in 10th House: Unexpected career twists; success in research, mining, espionage, or investigative fields.", "8th Lord in 11th House: Sudden massive financial gains or losses through friends; forms highly secretive, elite networks.", "8th Lord in 12th House: VIPAREETA RAJAYOGA. Profound spiritual liberation, isolated research, and conquering of inner fears."},
    {"", "9th Lord in 1st House: Natural-born luck; your physical body and reputation are fiercely protected by divine grace.", "9th Lord in 2nd House: Massive wealth creation through highly ethical means, teaching, or higher education.", "9th Lord in 3rd House: Philosophical mindset, fortunate short travels, and highly successful/lucky siblings.", "9th Lord in 4th House: Divine grace regarding properties; acquires real estate easily; highly religious mother.", "9th Lord in 5th House: Brilliant past-life karma; highly educated children, and immense luck in arts/investments.", "9th Lord in 6th House: Luck in legal or medical fields; physical obstacles are easily bypassed through divine grace.", "9th Lord in 7th House: Highly fortunate marriage; massive success through foreign partnerships and public dealings.", "9th Lord in 8th House: Luck in mystical sciences; unexpected inheritances and heavily protected spiritual transformations.", "9th Lord in 9th House: Absolute Dharma. Immense luck, high morality, father's unwavering support, and continuous divine grace.", "9th Lord in 10th House: DHARMA KARMADHIPATI YOGA. A highly ethical career, immense public respect, and top-tier leadership.", "9th Lord in 11th House: Fortunate financial gains, elite highly-placed networks, and effortless fulfillment of high ambitions.", "9th Lord in 12th House: Supreme spiritual Moksha; luck abroad, and massive success in ashrams or charitable trusts."},
    {"", "10th Lord in 1st House: Career defines your personality; a self-made, highly recognized, and ambitious leader.", "10th Lord in 2nd House: Direct, massive wealth creation through government connections, status, and your profession.", "10th Lord in 3rd House: Relentless career drive; success comes through communication, media, and frequent travels.", "10th Lord in 4th House: Building a career through real estate or from the hometown; holds great influence over the masses.", "10th Lord in 5th House: Creative, advisory, or speculative career; displays brilliant intellect in the workplace.", "10th Lord in 6th House: Career in medicine, law, military, or HR; utterly defeats enemies and competitors in the office.", "10th Lord in 7th House: Massive business partnerships, public relations, and foreign trade bring immense career success.", "10th Lord in 8th House: Sudden career changes; success in research, mining, investigation, or crisis management.", "10th Lord in 9th House: DHARMA KARMADHIPATI YOGA. Highest career peaks through higher knowledge, foreign travels, and teaching.", "10th Lord in 10th House: Unshakeable authority, massive public status, and absolute, unchallenged leadership.", "10th Lord in 11th House: Massive financial expansion through professional networks; perfect fulfillment of career goals.", "10th Lord in 12th House: Top-tier career in isolated places (hospitals, labs) or working for foreign multinational corporations."},
    {"", "11th Lord in 1st House: Ambitions manifest easily into the physical realm; naturally wealthy, highly networked leader.", "11th Lord in 2nd House: Unstoppable wealth creation, massive bank balance, and an ever-expanding family business.", "11th Lord in 3rd House: Gains through media, siblings, and continuous, competitive networking.", "11th Lord in 4th House: Massive property gains through real estate, luxury vehicles, agriculture, or the mother.", "11th Lord in 5th House: Incredible profits through speculation, stock markets, entertainment, and intelligent children.", "11th Lord in 6th House: Gains specifically through overcoming litigation, service industries, banking, or medical fields.", "11th Lord in 7th House: Profits through joint ventures, foreign trade, government contracts, and directly through the spouse.", "11th Lord in 8th House: Sudden, massive financial gains, or unearned profits through mystical and occult sciences.", "11th Lord in 9th House: Gains through foreign channels, higher education, and publishing; desires fulfilled purely by luck.", "11th Lord in 10th House: Massive career growth, corporate profits, and high status achieved through professional networks.", "11th Lord in 11th House: Absolute fulfillment of material desires; creates a highly powerful, profitable, and elite social circle.", "11th Lord in 12th House: Massive gains are spent on foreign investments, charity, or spiritual ashrams."},
    {"", "12th Lord in 1st House: Detachment from the material world, interest in foreign residence, and a highly spiritual, isolated nature.", "12th Lord in 2nd House: Wealth melts away through heavy expenses and charity, or indicates physical distance from the family.", "12th Lord in 3rd House: Depletion of courage or physical distance from siblings; highly secretive and isolated communication.", "12th Lord in 4th House: Detachment from the birthplace; settling permanently in foreign lands; creates a deeply spiritual domestic life.", "12th Lord in 5th House: Detachment from standard romance or progeny; grants deep, spiritual, and highly imaginative intellect.", "12th Lord in 6th House: VIPAREETA RAJAYOGA. Conquers enemies and debts through isolation, hidden strategies, or foreign help.", "12th Lord in 7th House: Foreign spouse, or physical distance/detachment within marriage and business partnerships.", "12th Lord in 8th House: VIPAREETA RAJAYOGA. Profound mastery of occult sciences, conquering of hidden fears, and deep spiritual depth.", "12th Lord in 9th House: Foreign travels for higher knowledge or Moksha; adopts highly spiritual but unconventional ideologies.", "12th Lord in 10th House: Working in isolated environments (hospitals, prisons) or building a career in foreign companies.", "12th Lord in 11th House: Builds massive networks abroad; majority of expenses are directed towards fulfilling personal desires.", "12th Lord in 12th House: Absolute Moksha. Deep spiritual liberation, profound inner peace in total isolation, and clearing all karmic debts."}
};

inline std::string get_lord_in_house_text(int lord_h, int placed_h) {
    if (lord_h >= 1 && lord_h <= 12 && placed_h >= 1 && placed_h <= 12) {
        return en_bhava_lord_matrix[lord_h][placed_h];
    }
    return "Neutral lordship placement.";
}

// =========================================================================
// RVS DEFAULTS AND NATURAL ENEMIES
// =========================================================================

const int rvs_bad_placements[10][12] = {
    {0,0,0,0,0,0,0,0,0,0,0,0}, 
    {0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0}, 
    {1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0}, 
    {0, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0}, 
    {1, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 1}, 
    {0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0}, 
    {1, 0, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0}, 
    {1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0}, 
    {1, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0}, 
    {0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0}  
};

const int natural_enemies[10][10] = {
    {0,0,0,0,0,0,0,0,0,0}, 
    {0, 0, 0, 0, 0, 0, 1, 1, 1, 1}, 
    {0, 0, 0, 0, 0, 0, 0, 0, 1, 1}, 
    {0, 0, 0, 0, 1, 0, 0, 0, 1, 1}, 
    {0, 0, 1, 0, 0, 0, 0, 0, 0, 0}, 
    {0, 0, 0, 0, 1, 0, 1, 0, 0, 0}, 
    {0, 1, 1, 0, 0, 0, 0, 0, 0, 0}, 
    {0, 1, 1, 1, 0, 0, 0, 0, 0, 0}, 
    {0, 1, 1, 1, 0, 0, 0, 0, 0, 0}, 
    {0, 1, 1, 1, 0, 0, 0, 0, 0, 0}  
};

// =========================================================================
// GOCHARA PHALA MATRIX (ENGLISH TRANSIT PREDICTIONS FROM MOON SIGN)
// =========================================================================

inline std::string get_gochar_text(int p_idx, int from_mo) {
    if (p_idx == 1) { // SURYA (Sun)
        switch(from_mo) {
            case 3: case 6: case 10: case 11:
                return "Highly Auspicious Transit. The bad times have passed. You will experience success in exams, interviews, and career. Great financial gains, government favors, and overall happiness for your family.";
            case 1: case 2: case 4: case 5: case 7: case 8: case 9: case 12:
                return "Challenging Transit. The Sun's position is currently unfavorable. You may experience unnecessary travel, physical fatigue, minor health issues (heat/eyes), or friction with authorities. Reduce your workload and stay vigilant.";
        }
    }
    else if (p_idx == 3) { // MANGAL (Mars)
        switch(from_mo) {
            case 3: case 6: case 11:
                return "Highly Auspicious Transit. You will exhibit immense courage and effortlessly defeat enemies or competitors. Excellent time for real estate gains, resolving debts, and experiencing robust health.";
            case 1: case 2: case 4: case 5: case 7: case 8: case 9: case 10: case 12:
                return "Volatile Transit. You may feel aggressive or easily irritated. Beware of heated arguments with family or your spouse. High caution is required regarding sudden accidents, cuts, or blood-pressure fluctuations.";
        }
    }
    else if (p_idx == 5) { // GURU (Jupiter)
        switch(from_mo) {
            case 1: return "Transit in Janma Rasi: Indicates relocation, mental distress, friction with superiors, and increased expenses.";
            case 2: return "Transit in 2nd House: Highly Auspicious. Brings family happiness, financial gains, great recognition, and wealth accumulation.";
            case 3: return "Transit in 3rd House: Indicates career obstacles, location changes, and minor disagreements with siblings.";
            case 4: return "Transit in 4th House: Brings domestic unrest, potential health issues for the mother, or worries regarding property.";
            case 5: return "Transit in 5th House: Highly Auspicious. Excellent time for marriage, childbirth, buying property, and massive business profits.";
            case 6: return "Transit in 6th House: Indicates health issues, debt pressures, or sudden fear/litigation from enemies.";
            case 7: return "Transit in 7th House: Highly Auspicious. Marriage for singles. Excellent business/career gains, health improvement, and marital bliss.";
            case 8: return "Transit in 8th House: Warns of illness, unexpected financial loss, fatigue during travel, and severe delays.";
            case 9: return "Transit in 9th House: Highly Auspicious. Immense fortune, pilgrimages, foreign travels, and overall wealth expansion.";
            case 10: return "Transit in 10th House: Indicates sudden career changes, fear of job loss, and potential business struggles.";
            case 11: return "Transit in 11th House: Highly Auspicious. Fulfillment of desires. Massive financial gains, promotions, and auspicious events.";
            case 12: return "Transit in 12th House: Indicates heavy expenses for auspicious events and long-distance travels.";
        }
    }
    else if (p_idx == 7) { // SHANI (Saturn)
        switch(from_mo) {
            case 1: return "Janma Shani (Middle of Sade Sati): Severe mental anxiety, physical fatigue, heavy workload, and potential illness.";
            case 2: return "2nd House Shani (End of Sade Sati): Financial constraints, family disputes, and unexpected financial losses.";
            case 3: return "3rd House Shani: Highly Auspicious. Increased courage, destruction of enemies, new job, and massive success.";
            case 4: return "4th House Shani (Ardhashtama): Domestic unrest, professional frustrations, and friction with relatives.";
            case 5: return "5th House Shani: Worries about children, investment delays, and misunderstandings in romance.";
            case 6: return "6th House Shani: Highly Auspicious. Absolute victory over court cases and enemies. Debts clear up, financial recovery.";
            case 7: return "7th House Shani: Severe misunderstandings with spouse, business partner disputes, and heavy travels.";
            case 8: return "8th House Shani (Ashtama): Sudden career obstacles, severe health issues, and unexpected hardships. Caution required.";
            case 9: return "9th House Shani: Father's health issues, detachment from religion/luck, and delays in higher studies.";
            case 10: return "10th House Shani: Immense workload in career. Friction with authorities, and a lack of recognition for hard work.";
            case 11: return "11th House Shani: Highly Auspicious. Permanent financial gains in all areas, and great social respect.";
            case 12: return "12th House Shani (Start of Sade Sati): Wasteful expenses, hidden fears, and unexpected travels.";
        }
    }
    else if (p_idx == 8 || p_idx == 9) { // RAHU & KETU
        if (from_mo == 3 || from_mo == 6 || from_mo == 11) return "Shadow Planet in Upachaya House: Highly favorable transit. Indicates sudden massive gains, foreign travels, and destruction of enemies.";
        else return "This shadow planet transit creates mental confusion, sudden changes, and karmic instability regarding the themes of this house.";
    }
    return "Standard transit results. May require routine effort or cause minor, short-term delays.";
}
// =========================================================================
// NEW: DYNAMIC VIMSHOTTARI DASHA PREDICTIONS (ENGLISH)
// =========================================================================

inline std::string get_dynamic_mahadasha(int p_idx, int score, int house) {
    std::string base = "";
    switch(p_idx) {
        case 1: base = "A period of rising authority, government favors, and leadership. Focus centers on career, ego, and your father."; break;
        case 2: base = "A deeply emotional and transitional phase. Brings focus to mental peace, mother, public life, arts, and frequent travels."; break;
        case 3: base = "A high-energy, fast-paced period. Excellent for acquiring real estate, property, and building courage. Beware of anger."; break;
        case 4: base = "A highly intellectual phase focusing on business, communication, and learning. Excellent for IT, networking, and writing."; break;
        case 5: base = "The golden era of growth, wisdom, and luck. Brings opportunities for marriage, childbirth, wealth accumulation, and deep religious respect."; break;
        case 6: base = "A long, luxurious period focused on comforts, romance, arts, and wealth. Excellent for acquiring vehicles and enjoying marital bliss."; break;
        case 7: base = "The period of karma, discipline, and hard work. Extreme patience will build a permanent, unshakable foundation for your life."; break;
        case 8: base = "A time of sudden, explosive changes. Brings intense worldly desires, potential foreign travel, and massive material gains."; break;
        case 9: base = "A period of detachment, spirituality, and internal reflection. Old chapters close so new ones can begin. Focus shifts away from materialism."; break;
    }

    std::string modifier = "";
    if (score >= 3) modifier += "This planet is highly dignified in your natal chart, meaning this phase will deliver its results with profound ease, bringing immense fortune, clarity, and structural growth. ";
    else if (score <= -2) modifier += "However, as this planet is afflicted or weakly placed, this phase demands immense patience. You may face delays, unexpected friction, or the need to actively remedy this planetary energy. ";

    if (house == 6 || house == 8 || house == 12) modifier += "Operating from a Dusthana (House of Transformation), this period strongly focuses on clearing karmic debts, overcoming sudden obstacles, and deep psychological or physical healing.";
    else if (house == 1 || house == 4 || house == 7 || house == 10) modifier += "Placed in a Kendra (Angular House), this period will be highly active, forcing major, visible developments in your core foundational life (career, home, or partnerships).";
    else if (house == 5 || house == 9) modifier += "Positioned in a Trikona (Trine), this period brings divine grace, natural luck, and the blossoming of past-life good karma.";
    else if (house == 3 || house == 11) modifier += "Placed in an Upachaya (House of Growth), this period starts with intense effort but compounds into massive gains and expanded networks over time.";
    
    return base + " " + modifier;
}

// =========================================================================
// ENGLISH 81-COMBINATION BHUKTI (ANTARDASHA) MATRIX - EVENT BASED
// =========================================================================

const char* const en_bhukti_matrix[10][10] = {
    {""}, // 0 index unused
    
    // 1: SUN MAHA DASHA
    {"",
     "Sun Bhukti: Expect a sudden rise in responsibilities and government/administrative favors. You may experience minor health issues related to body heat, headaches, or bone fatigue. A period where your ego and public image are highly visible.",
     "Moon Bhukti: A time of public recognition and mental clarity. You will likely acquire new comforts or vehicles. Relations with your mother improve, but you may experience slight emotional fluctuations at the workplace.",
     "Mars Bhukti: Very high energy and physical stamina. You will actively resolve pending property or land disputes. Warning: Be highly cautious of fire, sharp objects, and sudden, heated arguments with authority figures.",
     "Mercury Bhukti: Excellent period for clearing debts, signing new contracts, and starting educational pursuits. Your communication becomes highly persuasive. Skin allergies or nervous tension may temporarily bother you.",
     "Jupiter Bhukti: A highly fortunate phase bringing birth of children, marriage proposals, or major promotions. You will gain respect in your community, engage in religious activities, and see a sudden expansion of wealth.",
     "Venus Bhukti: Conflicts between professional duties and personal luxuries. You will spend heavily on home decorations, vehicles, or your spouse. Minor disagreements in marriage may arise due to ego issues.",
     "Saturn Bhukti: A period of intense friction and delays. You will face heavy workloads, opposition from superiors, and potential public humiliation if careless. Protect your physical health against chronic fatigue and joint pains.",
     "Rahu Bhukti: Sudden, unpredictable changes in career. You might undertake unexpected foreign travel or deal with highly deceptive people. Guard your reputation against false allegations and avoid taking unnecessary political risks.",
     "Ketu Bhukti: A frustrating period where career efforts feel unrecognized. You may experience sudden detachment from your job, minor physical injuries, or a strong desire to abandon material pursuits for spiritual isolation."
    },
    
    // 2: MOON MAHA DASHA
    {"",
     "Sun Bhukti: A brilliant period where emotional plans turn into authoritative actions. You will defeat competitors, gain favors from government officials, and see a clear improvement in your father's health and your own status.",
     "Moon Bhukti: Deep focus on the home, mother, and personal peace. You will likely purchase luxury items, enjoy good food, and spend time with family. However, the mind is highly sensitive and prone to cold/cough issues.",
     "Mars Bhukti: Rapid financial gains through real estate or technical work. Your mind will be restless and aggressive. Warning: Avoid impulsive financial decisions and be cautious of blood-related issues or sudden cuts.",
     "Mercury Bhukti: A joyful, intellectually stimulating period. You will succeed in business, writing, and networking. You will easily convince others with sweet speech, making it a great time for trade and commerce.",
     "Jupiter Bhukti: Absolute mental peace and divine grace. You will likely celebrate auspicious functions at home, acquire wealth effortlessly, and experience deep spiritual satisfaction. Excellent for childbirth and marriage.",
     "Venus Bhukti: A period of immense romance, social gatherings, and acquisition of fine clothes/jewelry. You will mix freely with others and enjoy great physical comforts, but must guard against overindulgence.",
     "Saturn Bhukti: Mental depression, lethargy, and feeling burdened by heavy family responsibilities. You may face delays in your work, separation from loved ones, and issues related to stomach or digestive health.",
     "Rahu Bhukti: An emotionally turbulent period filled with phobias, strange dreams, and sudden anxieties. You may face deception from close friends or undertake sudden, poorly planned travels. Guard against food poisoning.",
     "Ketu Bhukti: Profound emotional detachment. You will feel a sudden urge to distance yourself from relatives and material comforts. An excellent time for meditation, but expect misunderstandings with maternal figures."
    },
    
    // 3: MARS MAHA DASHA
    {"",
     "Sun Bhukti: Exceptional courage and dominance in the workplace. You will crush your enemies and gain a high-ranking position. Beware of fevers, high blood pressure, and dominating your family members too harshly.",
     "Moon Bhukti: Wealth accumulation through sudden, aggressive actions. You will buy property or vehicles. Your mind will be highly active, leading to both great financial success and restless, sleepless nights.",
     "Mars Bhukti: Extreme physical vitality. You will actively pursue real estate, construction, or technical projects. Warning: You are highly accident-prone now. Strictly avoid rash driving, weapons, and explosive materials.",
     "Mercury Bhukti: A clash of action and intellect. You will win debates and legal cases, but may face severe anxiety, skin issues, or harsh arguments with younger siblings and neighbors.",
     "Jupiter Bhukti: Bold actions supported by divine luck. You will succeed in massive projects, gain immense wealth, and possibly go on a pilgrimage. Health improves, and you will command great respect in society.",
     "Venus Bhukti: Intense passion and sudden, heavy expenditures on luxuries or the opposite sex. You may experience extreme highs and lows in romantic relationships. Beware of eye infections or urinary issues.",
     "Saturn Bhukti: Extreme frustration. Your drive to move fast is completely blocked by Saturn's delays. You will face heavy physical labor, workplace politics, and potential bone or joint injuries. Extreme patience is required.",
     "Rahu Bhukti: A highly dangerous, explosive period. You will take massive, unconventional risks. Warning: Strictly avoid illegal activities, weapons, and toxic substances. Hidden enemies will actively try to sabotage you.",
     "Ketu Bhukti: Sudden, unseen cuts and separations. You may face sudden surgeries, electrical shocks, or fire accidents. A period of intense frustration where your efforts yield zero visible results."
    },
    
    // 4: MERCURY MAHA DASHA
    {"",
     "Sun Bhukti: Sharp administrative success. You will receive honors from the government, pass competitive exams, and easily defeat rivals using pure intellect and strategy. Good health and high energy.",
     "Moon Bhukti: A highly creative but mentally exhausting period. You will excel in arts, writing, and public speaking, but may suffer from severe overthinking, anxiety, and skin or nerve-related health issues.",
     "Mars Bhukti: Aggressive communication. You will be highly argumentive, leading to disputes with family and siblings. However, this is an excellent time for technical learning, coding, and logical problem-solving.",
     "Mercury Bhukti: Outstanding period for business expansion, accounting, and learning new languages/skills. Your social network will expand massively, bringing profitable trade, good health, and joyful family gatherings.",
     "Jupiter Bhukti: A peak period for higher education, financial consulting, and publishing. You will meet wise mentors, gain massive wealth through intellectual means, and enjoy an incredibly peaceful domestic life.",
     "Venus Bhukti: Joyful travels, artistic success, and romantic developments. You will spend money on beautiful home interiors, music, and social events. Highly favorable for marriage and acquiring luxury vehicles.",
     "Saturn Bhukti: Slow, structured, and highly disciplined business growth. You will face delays, but your extreme focus will build a permanent financial empire. Expect minor nerve or joint pains due to overworking.",
     "Rahu Bhukti: Massive, explosive success in technology, mass media, or foreign trade. However, you must be extremely careful of signing fraudulent contracts, digital scams, or experiencing sudden nervous breakdowns.",
     "Ketu Bhukti: Mental fog and deep intuition. You will lose interest in standard business and shift towards astrology, coding, or hidden research. Expect sudden miscommunications and memory lapses."
    },
    
    // 5: JUPITER MAHA DASHA
    {"",
     "Sun Bhukti: A highly honorable period. You will gain a senior, respected position, receive favors from the government, and your father's health and status will improve. A time of absolute public dignity.",
     "Moon Bhukti: Deep domestic bliss and acquisition of wealth. You will experience profound mental peace, perform religious ceremonies at home, and easily acquire new vehicles and beautiful garments.",
     "Mars Bhukti: Courage meets luck. You will easily win legal battles, purchase vast real estate, and defeat your enemies. Your energy levels are high, and your actions will bring immediate, profitable results.",
     "Mercury Bhukti: Outstanding for financial consulting, teaching, and astrology. You will make brilliant investment decisions, expand your business networks, and enjoy a highly peaceful, intellectual family environment.",
     "Jupiter Bhukti: The golden era. Ultimate expansion of wealth, health, and happiness. You will gain immense social respect, perform charitable acts, and see the successful birth of children or a highly auspicious marriage.",
     "Venus Bhukti: A period of immense luxury and comfort. You will acquire precious jewelry, enjoy excellent food, and experience a highly romantic, blissful married life. Wealth flows in effortlessly.",
     "Saturn Bhukti: A period that demands hard work to maintain your wealth. You will build permanent, structural assets, but will face a heavy workload, lethargy, and a temporary slowdown in your usual luck.",
     "Rahu Bhukti: A clash of beliefs. You will experience sudden, massive expansion, often through foreign or unorthodox means. You may question your traditional religion and must guard against sudden liver or digestive issues.",
     "Ketu Bhukti: Complete spiritual detachment. You will embark on pilgrimages, give to charity, and distance yourself from material wealth. A highly peaceful but isolating period focusing entirely on inner liberation."
    },
    
    // 6: VENUS MAHA DASHA
    {"",
     "Sun Bhukti: A clash between duty and luxury. You will experience eye strain, headaches, and sudden conflicts with government or authority figures. Your luxury expenses will drain your savings.",
     "Moon Bhukti: A deeply romantic, highly aesthetic period. You will focus entirely on home decorations, perfumes, arts, and emotional comforts. Highly favorable for acquiring luxury vehicles and maternal happiness.",
     "Mars Bhukti: Explosive passion and high spending. You will actively pursue romantic interests and buy expensive properties. Warning: Guard against sudden, heated arguments in marriage and potential blood-related health issues.",
     "Mercury Bhukti: Joyful social interactions and business success through arts or design. You will make highly profitable investments, enjoy short luxurious trips, and maintain an excellent, humorous dynamic with friends.",
     "Jupiter Bhukti: Two gurus collide. You will gain immense wealth, perform religious rites, and experience marriage/childbirth. However, you may feel an internal conflict between spending on luxury versus giving to charity.",
     "Venus Bhukti: The absolute peak of material comfort. You will acquire the finest clothes, vehicles, and properties. A highly romantic, physically comfortable period with an uninterrupted flow of massive wealth.",
     "Saturn Bhukti: A legendary combination. Saturn rewards your past efforts with massive, highly stable, and permanent wealth. You will build a luxurious empire, but you must work extremely hard to maintain it.",
     "Rahu Bhukti: Intense, obsessive desire for materialism. You will experience sudden, massive financial windfalls, unconventional romantic affairs, and highly luxurious foreign travels. Beware of scandals or toxic indulgences.",
     "Ketu Bhukti: A sudden brake on luxury. You will experience a bizarre detachment from your wealth and physical relationships. A period of strange, unexplainable health issues and a desire to isolate yourself from society."
    },
    
    // 7: SATURN MAHA DASHA
    {"",
     "Sun Bhukti: Severe workplace friction. You will face heavy opposition from your bosses, father, or government. A period of false allegations, physical exhaustion, and potential issues related to heart or bone health.",
     "Moon Bhukti: Deep mental depression and isolation. You will feel burdened by massive family responsibilities, face delays in every project, and suffer from lethargy, cold-related diseases, and a lack of emotional support.",
     "Mars Bhukti: Extreme frustration and danger. Your desire to move fast is completely blocked. Warning: High risk of accidents, surgeries, fractures, and severe property disputes. Strict caution is advised in all physical activities.",
     "Mercury Bhukti: Structured, slow, and systematic success. You will build long-term business networks, excel in deep analytical research, and see a steady rise in income through extreme discipline and patience.",
     "Jupiter Bhukti: Relief from the heavy grind. Saturn allows Jupiter's grace to manifest, bringing a slow but steady recovery of wealth, resolution of legal disputes, and a mature, deeply philosophical mindset.",
     "Venus Bhukti: Highly prosperous. Your intense hard work finally pays off with massive, permanent luxury. You will acquire homes, vehicles, and experience a highly stable, deeply committed romantic/marital life.",
     "Saturn Bhukti: The crucible of discipline. A slow, heavy, and exhausting period that forces you to build structure. You will face delays, heavy manual or mental labor, and must strictly avoid laziness to succeed.",
     "Rahu Bhukti: A highly chaotic, cursed energy. You will face sudden, explosive changes, heavy physical exhaustion, and unpredictable karmic events. You must stay highly ethical to avoid sudden downfalls and hidden enemies.",
     "Ketu Bhukti: Complete karmic detachment. You will experience a massive loss of interest in your career and worldly duties. A period of wandering, isolation, and shedding old, useless structures from your life."
    },
    
    // 8: RAHU MAHA DASHA
    {"",
     "Sun Bhukti: Eclipse energy. Your reputation is at sudden risk. You may face political illusions, sudden transfers in your job, or massive, unpredictable shifts in your career trajectory. Guard against fevers and heart issues.",
     "Moon Bhukti: Intense emotional volatility. You will experience deep phobias, strange dreams, and sudden anxieties. However, it is an excellent period for massive success in foreign lands, imports/exports, and imaginative arts.",
     "Mars Bhukti: Explosive, dangerous ambition. You will take massive risks. Warning: Strictly avoid weapons, fire, and illegal activities. You are highly prone to sudden accidents, surgeries, and violent disputes.",
     "Mercury Bhukti: Brilliant but manipulative intellect. You will experience massive, sudden success in technology, media, and foreign business networks. You will easily outsmart your enemies using highly unconventional logic.",
     "Jupiter Bhukti: Sudden, massive expansion through highly unorthodox means. You will gain immense wealth, but you will constantly question traditional religion, clash with mentors, and experience bizarre, unexplainable luck.",
     "Venus Bhukti: Obsessive, extreme luxury. You will experience intense, unconventional romantic affairs, sudden massive material gains, and high indulgence in physical pleasures. Beware of scandals and overspending.",
     "Saturn Bhukti: A grinding, intensely stressful period where your wild ambitions hit a brick wall of reality. You will face heavy labor, chronic health issues, and severe karmic backlash if you have engaged in unethical actions.",
     "Rahu Bhukti: The absolute peak of illusion and ambition. You will experience sudden, massive changes, frequent foreign travels, and an intense obsession with worldly success. Expect sudden windfalls and equally sudden expenses.",
     "Ketu Bhukti: Karmic whiplash. The head and tail of the dragon pull you apart. You will face sudden stops, confusing reversals, mysterious health issues, and a chaotic, deeply confusing internal spiritual crisis."
    },
    
    // 9: KETU MAHA DASHA
    {"",
     "Sun Bhukti: Ego dissolution. You will experience a sudden loss of confidence, position, or authority. A frustrating time where your efforts go completely unnoticed. A period to surrender your ego and focus on your soul.",
     "Moon Bhukti: Profound emotional detachment. You will feel deeply isolated from your family and mother. Highly intuitive and spiritually transformative, but highly prone to loneliness, depression, and strange phobias.",
     "Mars Bhukti: Sudden, unseen cuts. Warning: High risk of sudden surgeries, electrical shocks, or unexpected detachment from your properties. You will fight invisible, highly frustrating battles behind the scenes.",
     "Mercury Bhukti: Intuitive logic. You will take a sharp break from standard business and communication. Excellent for diving into astrology, deep coding, and hidden research. Expect frequent miscommunications and memory lapses.",
     "Jupiter Bhukti: Pure spiritual grace. A highly auspicious, peaceful time for pilgrimages, deep philosophical learning, and ashram life. You will be protected from all major harms by an unseen, divine force.",
     "Venus Bhukti: Total detachment from pleasure. Romantic relationships will feel cold and distant. You will ignore physical luxuries and aesthetics in favor of internal peace and spiritual isolation.",
     "Saturn Bhukti: Depressive, heavy isolation. A highly restrictive period demanding extreme patience. You will face chronic, hard-to-diagnose physical pains, severe delays, and a feeling of being completely trapped.",
     "Rahu Bhukti: Karmic whiplash. Sudden, highly confusing events. You will be torn between extreme worldly ambition and extreme spiritual detachment, leading to massive internal chaos and sudden, unpredictable travels.",
     "Ketu Bhukti: The ultimate period of spiritual detachment. You will completely shed your old material identities, isolate yourself from society, and experience profound, highly psychic spiritual insights."
    }
};

inline std::string get_dynamic_bhukti(int md_idx, int ad_idx, int score, int house) {
    std::string base = en_bhukti_matrix[md_idx][ad_idx];
    std::string modifier = "";
    
    if (score >= 3) modifier += " | DIGNITY STRENGTH: The Bhukti lord is powerful in your natal chart, ensuring these results manifest smoothly and bring decisive success.";
    else if (score <= -2) modifier += " | DIGNITY AFFLICTION: The Bhukti lord is weak or afflicted. Expect significant delays, heightened stress, or obstacles regarding these themes.";
    
    if (house == 6 || house == 8 || house == 12) modifier += " Operating from a Dusthana, it triggers karmic cleanup, health focus, or sudden transitions.";
    else if (house == 1 || house == 4 || house == 7 || house == 10) modifier += " Operating from a Kendra, it forces major, highly visible actions in your foundational life.";
    
    return base + modifier;
}

// =========================================================================
// NEW: DYNAMIC LIFE-EVENT INJECTION (BHAVA LORDSHIPS) - ENGLISH
// =========================================================================

inline std::string get_house_theme(int h) {
    switch(h) {
        case 1: return "your core physical health and major new life beginnings";
        case 2: return "family wealth accumulation and savings";
        case 3: return "short travels, intense self-effort, and siblings";
        case 4: return "real estate, vehicles, and domestic peace";
        case 5: return "romance, children, and speculative investments";
        case 6: return "clearing debts, health routines, and overcoming workplace enemies";
        case 7: return "marital status, business partnerships, and public dealings";
        case 8: return "sudden transformations, hidden wealth, and deep psychological changes";
        case 9: return "long-distance travels, higher education, and sheer luck";
        case 10: return "career milestones, public status, and authority";
        case 11: return "massive financial gains and the fulfillment of major desires";
        case 12: return "foreign connections, heavy expenditures, and spiritual isolation";
        default: return "";
    }
}

inline std::string get_lordship_bhukti_event(std::string p_name, const std::vector<int>& houses) {
    if (houses.empty()) return "";
    if (houses.size() == 1) {
        return "Because " + p_name + " rules your " + std::to_string(houses[0]) + "th House, this period will physically manifest as events related to " + get_house_theme(houses[0]) + ".";
    } else {
        return "Because " + p_name + " rules your " + std::to_string(houses[0]) + "th and " + std::to_string(houses[1]) + "th Houses, this period will physically manifest as major developments involving " + get_house_theme(houses[0]) + " as well as " + get_house_theme(houses[1]) + ".";
    }
}

inline std::string get_node_bhukti_event(std::string p_name, int placed_house) {
    return "As a shadow node placed in your " + std::to_string(placed_house) + "th House, " + p_name + " will unpredictably amplify events related to " + get_house_theme(placed_house) + " during this time.";
}
