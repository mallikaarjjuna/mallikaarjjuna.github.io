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
// AI TEXT GENERATION MODULES
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

// MASSIVE 108-ITEM 2D KNOWLEDGE MATRIX (9 Planets x 12 Houses)
const char* const planet_in_house_matrix[10][13] = {
    // 0: Lagna (Unused placeholder)
    {"", "", "", "", "", "", "", "", "", "", "", "", ""},
    
    // 1: Surya (Sun)
    {"", 
     "The Sun in the 1st House creates a highly independent, radiant, and authoritative personality. The native naturally seeks leadership but must guard against ego clashes and physical heat/burnout.",
     "The Sun in the 2nd House illuminates wealth and family lineage. It grants an authoritative tone of speech and gains through government or authority figures, though it can cause friction in close family matters.",
     "The Sun in the 3rd House is extremely powerful, granting immense courage, willpower, and the absolute ability to crush competitors. However, it often creates ideological friction with younger siblings.",
     "The Sun in the 4th House brings a deep, private desire for internal power and control over the home environment. It can cause a restless domestic life and frequent emotional anxieties.",
     "The Sun in the 5th House creates a brilliantly sharp intellect and a flair for politics or speculation. It indicates high intelligence but often causes delays or ego-friction regarding children.",
     "The Sun in the 6th House is a conqueror placement. The native effortlessly defeats enemies, excels in litigation, and often succeeds in medical, competitive, or service-oriented fields.",
     "The Sun in the 7th House projects authority into partnerships. The native often attracts a dominant or high-status spouse, leading to potential power struggles and ego clashes in marriage or business.",
     "The Sun in the 8th House places the soul in the realm of the hidden. It grants profound research abilities and an interest in the occult, but warns of sudden transformations and issues with government/taxes.",
     "The Sun in the 9th House indicates a deeply philosophical, righteous, and ideological nature. The native holds high ideals, often leading to clashes with the father or established traditional gurus.",
     "The Sun in the 10th House gains maximum directional strength. This is a supreme placement for executive power, public visibility, massive career success, and seamless interaction with government or authorities.",
     "The Sun in the 11th House guarantees immense material gains through large networks, influential friends, and authority figures. It makes the native highly ambitious and successful in achieving their desires.",
     "The Sun in the 12th House separates the ego from the material world. It indicates success in foreign lands, hidden or isolated career paths (like hospitals or labs), and a deep, underlying spiritual quest."
    },

    // 2: Chandra (Moon)
    {"",
     "The Moon in the 1st House creates a magnetic, empathetic, and highly adaptable personality. The native's mind and physical body are deeply intertwined, making them highly responsive to their environment.",
     "The Moon in the 2nd House creates a soft, persuasive speaker whose financial state fluctuates with their emotional state. Wealth often comes through public-facing roles, food, or liquid assets.",
     "The Moon in the 3rd House creates a highly communicative and restless mind. The native loves short travels, writing, and networking, maintaining deep emotional bonds with siblings and neighbors.",
     "The Moon in the 4th House gains maximum directional strength. It grants profound internal peace, deep attachment to the mother and homeland, and a natural affinity for real estate and domestic comfort.",
     "The Moon in the 5th House blesses the native with an intensely creative and romantic mind. It indicates strong emotional intelligence, a love for the arts, and deep affection for children.",
     "The Moon in the 6th House places the emotional mind in a battlefield. The native is prone to anxiety and fluctuating health, but excels in service-oriented roles, healing, and resolving disputes.",
     "The Moon in the 7th House seeks deep emotional fulfillment through marriage and business partnerships. The native often marries young or attracts a sensitive, public-facing, or emotionally volatile spouse.",
     "The Moon in the 8th House forces the mind to navigate hidden depths. It grants intense intuition and psychological insight, but makes the native prone to sudden emotional turbulence and hidden fears.",
     "The Moon in the 9th House is highly auspicious, blessing the native with a naturally graceful, philosophical, and optimistic mind. It indicates deep faith, long travels, and profound support from the father/gurus.",
     "The Moon in the 10th House connects the emotional mind to public life. Career trajectory may be highly fluctuating or involve constant interaction with the masses, public relations, healing, or travel.",
     "The Moon in the 11th House grants a vast network of friends and highly fruitful social connections. The native achieves their desires smoothly and maintains a constantly expanding social circle.",
     "The Moon in the 12th House creates a deeply imaginative, private, and spiritually inclined mind. The native requires frequent isolation to recharge and often finds emotional peace in foreign lands or ashrams."
    },

    // 3: Mangal (Mars)
    {"",
     "Mars in the 1st House creates a fiercely independent, highly energetic, and physically robust native. It grants an aggressive, pioneering spirit but can make the native prone to impulsiveness and head injuries.",
     "Mars in the 2nd House brings intense drive towards wealth accumulation, often through technical or real estate means. It creates a harsh, direct, and argumentative tone of speech.",
     "Mars in the 3rd House is extremely powerful. It grants unstoppable willpower, athletic prowess, and a fearless nature. The native crushes obstacles but may fiercely dominate their younger siblings.",
     "Mars in the 4th House disrupts domestic peace with aggressive or controlling energy. It creates a strong drive to acquire real estate and properties, but indicates friction with the mother and internal unrest.",
     "Mars in the 5th House creates a brilliant, deeply analytical, and competitive intellect. The native loves strategy and sports, but may experience aggressive friction in romances or delays regarding children.",
     "Mars in the 6th House is a flawless placement for destroying enemies. The native is a natural fighter, excelling in litigation, surgery, or competitive environments, completely overpowering their opposition.",
     "Mars in the 7th House injects highly volatile and aggressive energy into partnerships. It creates dominant business alliances and extreme passion, but warns of severe friction and arguments in marriage.",
     "Mars in the 8th House creates a fearless approach to the unknown. It grants deep research abilities and intense transformative energy, but warns of sudden accidents, surgeries, or unexpected upheavals.",
     "Mars in the 9th House makes the native an aggressive defender of their own ideology or religion. They fiercely debate philosophies and possess a highly independent, self-made approach to their dharma.",
     "Mars in the 10th House gains maximum directional strength. This creates an unstoppable, highly ambitious executive. The native dominates their career field with ruthless efficiency, technical skill, and commanding authority.",
     "Mars in the 11th House is highly auspicious for wealth. It grants massive, aggressively acquired gains, highly ambitious goals, and a commanding presence within large social or corporate networks.",
     "Mars in the 12th House directs aggressive energy inward or toward foreign matters. It indicates heavy expenditures, hidden enemies, restless sleep, and a tendency to fight unseen battles."
    },

    // 4: Budha (Mercury)
    {"",
     "Mercury in the 1st House gains maximum directional strength. It creates an exceptionally youthful, highly adaptable, and brilliant native who thrives on data, logic, and rapid communication.",
     "Mercury in the 2nd House creates a flawless, persuasive, and often highly humorous speaker. Wealth is accumulated through business acumen, consulting, writing, or intellectual pursuits.",
     "Mercury in the 3rd House feels right at home, granting phenomenal writing, marketing, and networking skills. The native has a highly active, curious mind and excellent relations with siblings.",
     "Mercury in the 4th House brings constant intellectual activity into the home environment. The native loves studying, home-based businesses, and maintains a highly communicative relationship with the mother.",
     "Mercury in the 5th House indicates profound academic intelligence and a rapid learning curve. The native excels in mathematics, programming, or speculative trading, and enjoys mentally stimulating romances.",
     "Mercury in the 6th House creates a razor-sharp, highly critical problem solver. The native excels at organizing chaos, winning disputes through logic, and analyzing data, though it can cause nervous anxiety.",
     "Mercury in the 7th House creates a strong desire for a highly intellectual, communicative, and youthful partner. Marriage is often treated like a logical partnership or friendly business alliance.",
     "Mercury in the 8th House grants a brilliant investigative mind. The native excels at uncovering secrets, deep research, astrology, and navigating complex financial structures or taxes.",
     "Mercury in the 9th House links logic to higher philosophy. The native loves higher education, long-distance travel, publishing, and analyzes religion or dharma through a highly intellectual, logical lens.",
     "Mercury in the 10th House creates a highly visible intellectual career. The native excels in commerce, administration, IT, or communications, smoothly navigating corporate ladders through strategic networking.",
     "Mercury in the 11th House is an excellent placement for business gains. The native maintains a massive network of acquaintances and achieves immense financial success through trade, logic, and communication.",
     "Mercury in the 12th House creates a highly active subconscious mind. The native excels at hidden research, foreign languages, or coding, but may struggle with overthinking or communicating their deepest thoughts."
    },

    // 5: Guru (Jupiter)
    {"",
     "Jupiter in the 1st House gains maximum directional strength, wrapping the native in an aura of grace, optimism, and profound wisdom. It protects the physical body and ensures a highly respected, noble life path.",
     "Jupiter in the 2nd House blesses the native with immense wealth, an expanding family lineage, and a highly truthful, philosophical tone of speech. Money flows easily through advisory or educational roles.",
     "Jupiter in the 3rd House expands the native's courage through wisdom. They are highly optimistic, excellent teachers or writers, and maintain a deeply supportive relationship with their siblings and peers.",
     "Jupiter in the 4th House brings massive blessings, peace, and expansion to the domestic life. The native enjoys large homes, a deeply spiritual or supportive mother, and profound internal contentment.",
     "Jupiter in the 5th House is the ultimate placement for higher intelligence, divine grace, and exceptional progeny. It indicates past-life good karma, profound advisory skills, and natural luck in speculation.",
     "Jupiter in the 6th House protects the native from enemies through wisdom and law rather than combat. It grants success in service, healing, or legal professions, though it can expand debts if not careful.",
     "Jupiter in the 7th House blesses the native with a highly noble, wise, and supportive spouse. It expands business networks naturally and ensures highly ethical, prosperous partnerships.",
     "Jupiter in the 8th House grants profound occult and psychological wisdom. It protects the native during sudden transformations, grants hidden inheritances, and creates a highly intuitive, secretive researcher.",
     "Jupiter in the 9th House is the pure embodiment of Dharma. It creates a deeply religious, highly educated native who naturally attracts excellent gurus, long-distance travel, and divine fortune.",
     "Jupiter in the 10th House creates a highly respected, ethical, and advisory career. The native easily attains positions of management, teaching, or legal authority, universally respected by society.",
     "Jupiter in the 11th House guarantees immense, expanding wealth and a highly influential network of powerful friends. The native effortlessly achieves their highest ambitions through grace and networking.",
     "Jupiter in the 12th House expands the native's spiritual dimensions. It grants deep peaceful sleep, successful foreign travel, massive success in ashrams or isolated work, and acts as a pure guardian angel."
    },

    // 6: Shukra (Venus)
    {"",
     "Venus in the 1st House creates an exceptionally beautiful, magnetic, and charismatic personality. The native possesses highly refined tastes, a love for luxury, and naturally attracts harmony and attention.",
     "Venus in the 2nd House creates a sweet, highly charming speaker who easily accumulates liquid wealth and luxury assets. The native enjoys fine foods, beautiful family environments, and artistic financial pursuits.",
     "Venus in the 3rd House brings grace and diplomacy to communication. The native excels in artistic hobbies, creative writing, and maintains highly affectionate, harmonious relationships with their siblings.",
     "Venus in the 4th House gains maximum directional strength, bringing profound luxury, vehicles, and beauty into the home. It ensures deep inner happiness, a loving mother, and a highly aesthetic domestic life.",
     "Venus in the 5th House creates a deeply romantic, highly creative, and artistic native. It indicates a love for entertainment, successful speculative investments in luxury, and beautiful, affectionate children.",
     "Venus in the 6th House forces the planet of harmony into the house of conflict. The native often works in fields related to healing, women, or resolving disputes, but must guard against health issues from overindulgence.",
     "Venus in the 7th House creates a profound desire for a beautiful, harmonious marriage. The native attracts a highly aesthetic or wealthy partner and excels effortlessly in public relations and business alliances.",
     "Venus in the 8th House grants a love for the mysterious and hidden. It indicates sudden financial gains through partners or inheritance, deep, intense secret romances, and a highly transformative life path.",
     "Venus in the 9th House blesses the native with a love for foreign cultures, higher philosophies, and long-distance travel. They experience natural grace, artistic higher education, and an effortless, lucky dharma.",
     "Venus in the 10th House creates a highly visible career involving arts, luxury, women, or diplomacy. The native is widely loved in their professional circle and easily attains status through charm and creativity.",
     "Venus in the 11th House is an ultimate wealth placement. It guarantees massive financial gains, a vast network of female or artistic friends, and the effortless fulfillment of all material and romantic desires.",
     "Venus in the 12th House is a unique placement where Venus thrives in the house of bed pleasures. It grants profound physical luxuries, success in foreign lands, and deep, imaginative, spiritual artistic abilities."
    },

    // 7: Shani (Saturn)
    {"",
     "Saturn in the 1st House creates a serious, highly disciplined, and mature personality from a young age. The native is hardworking and realistic, but must guard against melancholy, self-doubt, or physical delays.",
     "Saturn in the 2nd House makes the native highly conservative with wealth and cautious in speech. Financial stability comes slowly but permanently through immense grinding effort and strict discipline.",
     "Saturn in the 3rd House is extremely powerful, granting unbreakable willpower, monumental patience, and the stamina to outlast any competitor. The native communicates seriously and dominates through endurance.",
     "Saturn in the 4th House forces early maturity and creates an emotionally detached or highly structured home environment. It grants lasting real estate later in life but requires overcoming deep internal anxieties.",
     "Saturn in the 5th House slows down the results of education, romance, and progeny, demanding extreme structure. The native possesses a deep, serious intellect suited for engineering, history, or hard sciences.",
     "Saturn in the 6th House is a phenomenal placement. The native slowly, methodically, and permanently crushes their enemies and diseases. It creates an unstoppable work ethic and massive success in service or legal fields.",
     "Saturn in the 7th House gains maximum directional strength. It delays marriage to ensure absolute stability, attracting an older, mature, or hardworking partner, and grants the stamina to build massive public networks.",
     "Saturn in the 8th House significantly increases the native's physical longevity. It creates a deeply serious, secretive researcher who must navigate heavy karmic transformations and structured occult knowledge.",
     "Saturn in the 9th House indicates a deeply traditional, rigid, or highly disciplined approach to religion and philosophy. It can cause early delays or friction with the father, forcing the native to build their own belief system.",
     "Saturn in the 10th House creates the ultimate slow-burning executive. Career rise is slow, demanding, and requires monumental effort, but leads to permanent, unshakable authority and massive public status.",
     "Saturn in the 11th House is highly auspicious, systematically building massive networks and permanent wealth over time. The native's desires and long-term goals are achieved with absolute certainty through delayed gratification.",
     "Saturn in the 12th House forces the native to confront isolation, loss, and hidden debts. It creates an incredibly disciplined spiritual seeker and often indicates massive, structured success in completely foreign lands or isolated environments."
    },

    // 8: Rahu (North Node)
    {"",
     "Rahu in the 1st House creates an intensely ambitious, unconventional, and highly charismatic personality. The native is obsessed with self-development and breaking boundaries, projecting a highly magnetic, illusionary aura.",
     "Rahu in the 2nd House creates an obsessive desire for massive wealth accumulation and unconventional assets. The native possesses a highly persuasive, sometimes manipulative voice, and must guard against unorthodox financial risks.",
     "Rahu in the 3rd House is a phenomenal placement. It grants explosive courage, extreme competitive drive, and brilliant, out-of-the-box communication skills. The native crushes all opposition through sheer ambition.",
     "Rahu in the 4th House creates a restless, unconventional home environment and a deep inner craving for massive properties. The native often relocates far from their birthplace or experiences intense emotional fluctuations.",
     "Rahu in the 5th House amplifies a brilliant, unorthodox, and highly speculative intellect. The native excels in modern technology, politics, or media, but experiences intense, unusual romances and potential illusions regarding children.",
     "Rahu in the 6th House is a flawless placement for material dominance. The native effortlessly destroys enemies, masters foreign or technical subjects, and completely overpowers any competitive or legal opposition.",
     "Rahu in the 7th House creates an intense obsession with partnerships and the public. The native often attracts foreign, unconventional, or highly ambitious spouses, leading to extreme highs and lows in marriage and business.",
     "Rahu in the 8th House plunges the native into the extremes of the occult, hidden wealth, and deep psychological research. It warns of sudden, massive transformations, unconventional secrets, and sudden financial windfalls or losses.",
     "Rahu in the 9th House creates a highly unorthodox, boundary-breaking approach to religion and philosophy. The native frequently travels long distances, studies foreign cultures, and builds their own unique, non-traditional dharma.",
     "Rahu in the 10th House is an explosive career placement. The native is obsessed with public status and achieves massive, sudden authority, frequently involving foreign elements, technology, politics, or highly visible mass manipulation.",
     "Rahu in the 11th House is the ultimate placement for sudden, massive material gains. The native builds a vast, powerful, and unconventional network of friends, effortlessly and aggressively manifesting their highest worldly ambitions.",
     "Rahu in the 12th House creates a highly active, imaginative, and sometimes chaotic subconscious mind. The native experiences extreme foreign travels, deep spiritual illusions, and intense, vivid dreams or hidden expenses."
    },

    // 9: Ketu (South Node)
    {"",
     "Ketu in the 1st House creates a deeply introverted, spiritual, and highly intuitive personality. The native feels detached from their physical body and worldly ego, possessing profound, unexplainable past-life insights.",
     "Ketu in the 2nd House creates a natural detachment from material wealth and family lineage. Money often comes and goes without the native's active effort, and their speech is often brutally blunt, philosophical, or highly cryptic.",
     "Ketu in the 3rd House grants sudden bursts of courage and a highly intuitive, sharp communication style. The native is detached from sibling politics and easily cuts through immediate obstacles with minimal effort.",
     "Ketu in the 4th House creates a profound feeling of detachment from the birthplace, mother, or physical home. The native seeks deep internal moksha and often feels like a stranger in their own domestic environment.",
     "Ketu in the 5th House brings past-life genius, especially in mathematics, coding, or ancient texts. However, it creates a deep detachment from modern romances, speculative risks, and conventional approaches to raising children.",
     "Ketu in the 6th House is an excellent placement for spiritual or medical service. The native is immune to the politics of enemies and quietly, almost invisibly, resolves disputes, though diseases can be mysterious and hard to diagnose.",
     "Ketu in the 7th House creates a deep, karmic detachment from marriage and public partnerships. The native often feels disconnected from their spouse or attracts highly spiritual, unconventional partners, requiring constant boundary setting.",
     "Ketu in the 8th House is the ultimate placement for deep occult mastery, astrology, and spiritual liberation. The native possesses extreme psychic intuition and a fearless detachment toward sudden transformations and the unknown.",
     "Ketu in the 9th House creates a naturally pure, deeply spiritual, and highly philosophical native. They possess innate past-life wisdom, often rejecting traditional religious dogmas in favor of absolute, direct spiritual truth.",
     "Ketu in the 10th House creates a native who is highly skilled but deeply detached from public status or corporate ladders. They often excel in highly specialized, isolated, or spiritual careers, feeling indifferent to worldly authority.",
     "Ketu in the 11th House creates a detachment from large social networks and typical material ambitions. The native achieves gains effortlessly due to past-life merit, but feels deeply indifferent to worldly status or superficial friendships.",
     "Ketu in the 12th House is the ultimate placement for Moksha (Spiritual Liberation). The native possesses profound subconscious clarity, requires deep isolation, and is actively burning off their final karmic debts in this lifetime."
    }
};

inline std::string get_planet_in_house_text(int p_idx, int h) {
    if (p_idx >= 1 && p_idx <= 9 && h >= 1 && h <= 12) {
        return planet_in_house_matrix[p_idx][h];
    }
    return "Neutral placement operating dynamically.";
}

inline std::string get_final_outcome(int p_idx, bool is_benefic, bool is_malefic, bool is_kendra_lord, bool is_dusthana, bool is_kendra_trikona, int h, std::string disp_name) {
    if (p_idx == 8 || p_idx == 9) { 
        if (h==3 || h==6 || h==10 || h==11) return "Highly Auspicious (Upachaya Node). Placed in a house of growth, this shadow planet will grow exceptionally strong over time, granting sudden and massive material success by aggressively channeling the energy of its dispositor, " + disp_name + ".";
        else if (is_dusthana) return "Highly Volatile (Dusthana Node). Placed in an unstable house, this shadow planet amplifies hidden fears, sudden expenses, or psychological volatility, operating entirely through the disrupted energy of " + disp_name + ".";
        else return "Karmic Amplifier. Placed in a core angular or trine house, this shadow planet heavily eclipses the standard energy here, amplifying its own karmic themes while relying entirely on the foundational strength of " + disp_name + " to deliver physical results.";
    } 
    
    if (is_malefic && is_dusthana) return "Vipareeta Raja Yoga (Mixed but Ultimately Victorious). Because a challenging lord is placed in a challenging house, the negatives mathematically cancel out. Expect significant early struggles, enemies, or debts that eventually reverse to trigger your greatest rise and success.";
    if (is_benefic && is_kendra_lord && is_dusthana) return "Delayed Reward (Auspicious but Slow). A highly supportive planet is placed in a house of loss or obstacles. Ultimate success will manifest, but only after intense patience, grinding effort, or physically relocating away from your place of birth.";
    if (is_benefic && is_kendra_trikona) return "Highly Auspicious (Excellent). Operating with excellent lordship and placed in a house of power or grace, this planet serves as a major, reliable pillar of support, bringing continuous fortune, intellect, and ease to your life.";
    if (is_malefic && is_kendra_trikona) return "Highly Volatile (Powerful but High Stress). This planet brings immense drive, extreme potential for authority, and raw power, but carries a difficult energetic job description. Expect sudden fluctuations, intense stress, or the need to aggressively enforce boundaries.";
    
    if (is_dusthana) return "Challenging (Karmic Focus). Confined to an obstacle house, this planet demands deep, uncomfortable transformation. It requires you to navigate hidden traumas and forces areas of literal loss to teach necessary detachment.";
    
    if (h==3 || h==6 || h==10 || h==11) return "Growth Oriented (Demanding but Rewarding). Operating from an Upachaya (growing) house, this planet provides steady gains and aids in building networks, but it will force you to work relentlessly and occasionally sacrifice peace to achieve your ambitions.";
    if (is_kendra_trikona) return "Supportive and Stabilizing (Neutral). Placed in a foundational house, this planet acts as a steady stabilizing force, bringing natural grace and unforced support to the native's life trajectory.";
    
    return "Neutral House Placement. Operates dynamically based on conjunctions and transits.";
}

// =========================================================================
// VARGA SYNTHESIS VOCABULARY (D1 to Dx Cross-Referencing)
// =========================================================================

inline int get_varga_karaka(int v_num) {
    switch(v_num) {
        case 2: return 5; // Jupiter (Wealth)
        case 3: return 3; // Mars (Courage/Siblings)
        case 4: return 2; // Moon (Properties/Peace)
        case 7: return 5; // Jupiter (Progeny)
        case 9: return 6; // Venus (Marriage/Dharma)
        case 10: return 4; // Mercury (Commerce/Career execution)
        case 12: return 1; // Sun (Parents/Lineage)
        case 16: return 6; // Venus (Luxuries/Vehicles)
        case 20: return 5; // Jupiter (Spirituality)
        case 24: return 4; // Mercury (Intellect/Education)
        case 27: return 3; // Mars (Physical Strength/Endurance)
        case 30: return 7; // Saturn (Misfortunes/Shadows)
        default: return -1;
    }
}

inline std::string get_vargottama_text(std::string p_name) {
    return p_name + " is VARGOTTAMA. By occupying the exact same sign in both the D1 and this micro-chart, its energy is permanently crystallized. This planet acts with supreme confidence, structural rigidity, and massive fated influence in this area of life.";
}

inline std::string get_d1_lord_in_varga_text(std::string d1_lord_name, int h) {
    if (h == 1 || h == 5 || h == 9) return "Highly Auspicious Alignment. Placed in a Grace/Dharma house (House " + std::to_string(h) + ") in this micro-chart, it shows your physical self naturally aligns with and thrives in this area of life.";
    if (h == 4 || h == 7 || h == 10) return "Heavy Action Demanded. Placed in an Angle/Kendra (House " + std::to_string(h) + ") in this micro-chart, it forces you to take heavy, direct, and highly visible action regarding these themes.";
    if (h == 6 || h == 8 || h == 12) return "Karmic Friction and Exhaustion. Placed in a Dusthana (House " + std::to_string(h) + ") in this micro-chart, it indicates that engaging with these themes will physically or mentally exhaust you, requiring deep transformation or literal loss.";
    return "Persistent Growth Required. Placed in an Upachaya house (House " + std::to_string(h) + "), your mastery over this area of life will start slow but grow massively through aggressive, persistent effort.";
}
// =========================================================================
// BHAVA LORDSHIP VOCABULARY (144 Classical BPHS Interpretations)
// =========================================================================

const char* const bhava_lord_matrix[13][13] = {
    {""}, // 0 index unused
    
    // Lord of 1st House (Lagna Lord)
    {"", 
     "1st House: Creates a highly independent, self-made individual with robust health and natural leadership.",
     "2nd House: The native's core focus is on wealth accumulation, family lineage, and commanding speech.",
     "3rd House: The native is fiercely courageous, artistic, and achieves success through immense self-effort.",
     "4th House: Deeply attached to the home and mother; acquires massive physical assets and inner peace.",
     "5th House: Highly intelligent and blessed with excellent progeny; carries profound past-life merit.",
     "6th House: Overcomes enemies and thrives in service or medical fields, but must guard against health struggles.",
     "7th House: Life is deeply intertwined with partnerships; indicates a successful marriage and public-facing career.",
     "8th House: Highly mystical and secretive; prone to sudden transformations, hidden wealth, and deep psychological resilience.",
     "9th House: Extremely fortunate and religious; heavily supported by divine grace, the father, and long-distance travels.",
     "10th House: Deeply career-driven; the native attains significant executive authority and self-made professional success.",
     "11th House: Achieves massive financial gains, builds large influential networks, and effortlessly fulfills worldly ambitions.",
     "12th House: Highly spiritual and charitable; tends to settle in foreign lands or isolated environments."
    },

    // Lord of 2nd House
    {"",
     "1st House: Wealth comes effortlessly to the native; highly family-oriented with a persuasive, commanding voice.",
     "2nd House: Phenomenal for wealth accumulation, fixed assets, and maintaining a powerful, unbroken family lineage.",
     "3rd House: Wealth is generated primarily through intense self-effort, communications, media, and siblings.",
     "4th House: Massive financial gains through real estate, the mother, and agricultural or domestic assets.",
     "5th House: Wealth generated through speculation, high intellect, investments, and successful children.",
     "6th House: Money is earned through service, law, or medicine; potential for family disputes over finances.",
     "7th House: Wealth acquired through business partnerships, foreign trade, or directly through the spouse.",
     "8th House: Hidden wealth, sudden inheritances, or extreme sudden financial fluctuations and hidden assets.",
     "9th House: Wealth generated through higher education, publishing, or receiving financial blessings from the father/gurus.",
     "10th House: High earning capacity achieved directly through career status, government, and executive authority.",
     "11th House: Massive compounding wealth, highly successful banking, and immense profits through large networks.",
     "12th House: Wealth is drained through heavy expenses, charity, or settling in completely foreign lands."
    },

    // Lord of 3rd House
    {"",
     "1st House: Highly self-made and fiercely courageous; physical body is utilized in active, competitive environments.",
     "2nd House: Courage and willpower are applied directly to acquiring wealth; gains achieved through siblings.",
     "3rd House: Immense willpower, highly communicative, and absolutely thrives in intense competition or media.",
     "4th House: Property gains through siblings; but indicates a highly active or restless domestic environment.",
     "5th House: Sharp, highly competitive intellect; excellent placement for success in sports, arts, or technical skills.",
     "6th House: Crushes enemies effortlessly; highly powerful placement for litigation, sports, and defeating rivals.",
     "7th House: Dominant and courageous in business partnerships; but warns of potential ego friction in marriage.",
     "8th House: Sudden aggressive transformations; highly research-oriented, secretive, and fearless in crises.",
     "9th House: Fights fiercely for ideology; indicates long journeys, philosophical debates, and an unorthodox dharma.",
     "10th House: Highly ambitious and driven career; massive success in media, technical fields, or the military.",
     "11th House: Massive gains achieved through bold actions, relentless networking, and elder siblings.",
     "12th House: Courage applied in foreign lands or isolated research; warns of potential hidden losses or secret enemies."
    },

    // Lord of 4th House
    {"",
     "1st House: Deeply attached to the mother; physical comforts, vehicles, and properties come naturally to the native.",
     "2nd House: Inherited property, massive family wealth, and successfully acquired ancestral assets.",
     "3rd House: Real estate acquired entirely through self-effort; often indicates moving away from the exact birthplace.",
     "4th House: Excellent properties, profound inner peace, deep domestic luxury, and immense maternal blessings.",
     "5th House: Highly educated, emotionally connected to children, and enjoys profound creative comforts.",
     "6th House: Disputes over property; success in medical or legal real estate fields; overcoming domestic enemies.",
     "7th House: Real estate gains through the spouse; indicates living abroad or acquiring highly successful business properties.",
     "8th House: Sudden loss or sudden inheritance of property; hidden domestic issues and deep psychological transformations.",
     "9th House: Fortune through real estate; indicates a highly religious, spiritual, or philosophically rich home life.",
     "10th House: Powerful public status achieved through properties, politics, or a highly successful home-based career.",
     "11th House: Multiple properties, a massive network of friends, and highly profitable long-term physical assets.",
     "12th House: Deep detachment from the homeland; settling in completely foreign lands or isolated, spiritual environments."
    },

    // Lord of 5th House
    {"",
     "1st House: Exceptionally intelligent, highly creative, and possesses a naturally charismatic, romantic physical presence.",
     "2nd House: Wealth acquired through high intellect, speculation, advisory roles, and a highly educated family.",
     "3rd House: Intellectual courage; massive success in media, writing, and brilliant relations with siblings.",
     "4th House: Deep happiness through higher education, the mother, and bringing profound intellect into the home.",
     "5th House: Brilliant mind, excellent progeny, and profound past-life merit (Poorva Punya). Natural luck in speculation.",
     "6th House: Intellectual dominance over enemies; massive success in competitive exams; potential friction regarding children.",
     "7th House: A romantic marriage to a highly intelligent spouse; success in foreign business and public advising.",
     "8th House: Deep occult knowledge, sudden psychological shifts, secretive romances, and intense research capabilities.",
     "9th House: Spiritual brilliance, higher learning, publishing, and profound philosophical wisdom. Extremely fortunate.",
     "10th House: Career in elite advisory, teaching, or creative execution; highly respected by authority figures.",
     "11th House: Massive financial gains through speculation, stock markets, and vast creative or political networks.",
     "12th House: Spiritual intelligence, foreign education, and deep imaginative capacity; potential physical distance from children."
    },

    // Lord of 6th House
    {"",
     "1st House: The native effortlessly overcomes physical ailments; success in service, military, or healing professions.",
     "2nd House: Wealth generated through medical, legal, or service fields; warns of potential family disputes over finances.",
     "3rd House: Fierce competitor; defeats enemies entirely through courage, logic, and highly aggressive communication.",
     "4th House: Domestic peace challenged by disputes; highly successful in real estate litigation or agricultural service.",
     "5th House: Highly analytical intellect; potential friction regarding investments or competitive environments for children.",
     "6th House: Harsha Yoga. Flawless victory over enemies, debts, and diseases. Unstoppable dominance in competition.",
     "7th House: Disputes in marriage; highly combative but legally successful business alliances and contracts.",
     "8th House: Vipareeta Raja Yoga. Sudden victory over massive hidden obstacles; surviving extreme crises to achieve success.",
     "9th House: Friction regarding religion or the father; immense success in foreign legal matters or long-distance disputes.",
     "10th House: Career dominance in medicine, law, military, or human resources; defeating rivals in the corporate ladder.",
     "11th House: Financial gains through overcoming obstacles; massive success and networking power after initial struggle.",
     "12th House: Vipareeta Raja Yoga. Massive success triggered by initial losses; crushing enemies through isolation or foreign help."
    },

    // Lord of 7th House
    {"",
     "1st House: The spouse is deeply intertwined with the native's physical life, core identity, and public image.",
     "2nd House: Wealth heavily generated through marriage or business alliances; massive family business success.",
     "3rd House: Courageous spouse; immense success in networking, media, and joint communications with partners.",
     "4th House: Spouse brings property and domestic peace; massive success in hometown business ventures.",
     "5th House: Love marriage; attracts a highly intelligent spouse; successful in speculative or creative partnerships.",
     "6th House: Marital friction or legal delays; spouse may be deeply involved in medical, service, or legal fields.",
     "7th House: Excellent marriage, highly successful business partnerships, and a magnetic, widely respected public image.",
     "8th House: Sudden transformations in marriage; gaining hidden or unearned wealth directly through the spouse.",
     "9th House: Spouse from a completely different culture; immense fortune, grace, and foreign travel after marriage.",
     "10th House: Massive career success achieved after marriage; working jointly with the spouse in public authority.",
     "11th House: Highly profitable business alliances; the spouse brings a massive, lucrative social network.",
     "12th House: Foreign spouse, settling abroad completely after marriage, or highly secretive business partnerships."
    },

    // Lord of 8th House
    {"",
     "1st House: Prone to sudden physical transformations; highly mystical, secretive, and resilient core nature.",
     "2nd House: Sudden fluctuations in wealth; access to deep ancestral secrets or massive hidden inheritances.",
     "3rd House: Courageous in the face of sudden traumas; highly secretive communications and intense willpower.",
     "4th House: Sudden changes in residence; uncovering hidden truths about the mother, real estate, or the homeland.",
     "5th House: Unconventional intellect, sudden unexpected shifts in romance, and deep mastery of occult studies.",
     "6th House: Vipareeta Raja Yoga. Overcomes massive crises, debts, and enemies to achieve sudden, striking success.",
     "7th House: Transformative or secretive marriage; sudden, powerful shifts in business alliances and public image.",
     "8th House: Extreme physical longevity, profound occult mastery, and deep psychological resilience against all crises.",
     "9th House: Unorthodox philosophy, highly transformative foreign travels, and deeply questioning traditional dharma/father.",
     "10th House: Hidden or sudden career shifts; massive success in research, mining, espionage, or investigative fields.",
     "11th House: Sudden, explosive financial windfalls, or sudden losses through friends; highly secretive elite networks.",
     "12th House: Vipareeta Raja Yoga. Deep spiritual liberation, isolated research, and conquering hidden fears successfully."
    },

    // Lord of 9th House
    {"",
     "1st House: A naturally blessed life; the physical body and ego are fully protected by divine grace and high fortune.",
     "2nd House: Massive wealth generated through ethical means, publishing, teaching, or profound higher knowledge.",
     "3rd House: Highly philosophical communications, highly fortunate short travels, and deeply lucky/spiritual siblings.",
     "4th House: Divine blessings on the home; acquiring grand properties easily; a deeply religious and supportive mother.",
     "5th House: Excellent past-life karma; highly intelligent children, and absolute divine luck in speculation and arts.",
     "6th House: Fortune in legal disputes or medicine; overcoming massive physical obstacles easily through divine grace.",
     "7th House: Highly fortunate marriage; immense success in foreign business alliances and ethical public dealings.",
     "8th House: Occult fortune; receiving unearned inheritances and experiencing deep, protected spiritual transformations.",
     "9th House: Pure Dharma. Extreme luck, high morality, profound fatherly support, and unbroken divine grace.",
     "10th House: Dharma Karmadhipati Yoga. A highly righteous career, massive public respect, and ethical executive leadership.",
     "11th House: Fortunate financial gains, elite highly-placed networks, and effortless fulfillment of high ambitions.",
     "12th House: Ultimate spiritual moksha; fortunate foreign settlements, and massive success in ashrams or charity."
    },

    // Lord of 10th House
    {"",
     "1st House: Career completely defines the core identity; the native is a self-made, highly visible, structural leader.",
     "2nd House: Massive wealth generated directly through executive status, government connections, and career authority.",
     "3rd House: Career requires immense willpower, short travels, media presence, and aggressive communication.",
     "4th House: Career built directly from the hometown or in real estate; exercises profound local public influence.",
     "5th House: Creative, elite advisory, or speculative career; demonstrates high, unmatched intelligence in execution.",
     "6th House: Career in human services, medicine, law, or the military; flawlessly overcoming workplace enemies.",
     "7th House: Career built through massive business partnerships, foreign trade, or highly magnetic public relations.",
     "8th House: Secretive or investigative career; experiences massive, sudden shifts in public status or works with the occult.",
     "9th House: Dharma Karmadhipati Yoga. Career driven by higher knowledge, foreign travel, teaching, and sheer fortune.",
     "10th House: Unshakable executive authority, highly successful public status, and commanding structural power.",
     "11th House: Professional networks yield massive financial expansion and the total fulfillment of all professional goals.",
     "12th House: Career in isolated environments (hospitals, prisons, labs) or highly successful foreign corporate ventures."
    },

    // Lord of 11th House
    {"",
     "1st House: Ambitions are physically manifested; the native is highly networked, inherently wealthy, and socially powerful.",
     "2nd House: Unstoppable wealth generation, massive bank balances, and a highly successful, expanding family business.",
     "3rd House: Gains achieved through media, siblings, and relentless, highly competitive networking.",
     "4th House: Massive gains through real estate, luxury vehicles, agriculture, and the mother's ancestral lineage.",
     "5th House: Exceptional profits from speculation, entertainment, stock markets, and brilliant, successful children.",
     "6th House: Gains specifically through overcoming litigation, service industries, banking, or medical fields.",
     "7th House: Massive profits through joint ventures, foreign trade, public contracts, and directly through the spouse.",
     "8th House: Sudden, explosive financial windfalls or the acquisition of massive hidden, untaxed, or occult wealth.",
     "9th House: Gains through foreign lands, higher education, publishing, and sheer unadulterated luck.",
     "10th House: Professional networks yield massive career expansion, corporate gains, and high executive status.",
     "11th House: Unbounded fulfillment of all material desires; commands a massive, powerful, and highly lucrative social circle.",
     "12th House: Spending massive gains on foreign investments, global charity, or secluded spiritual ashrams."
    },

    // Lord of 12th House
    {"",
     "1st House: Physical detachment, a strong tendency for foreign residence, and a highly spiritual, isolated core identity.",
     "2nd House: Wealth is naturally drained through expenses, charity, or indicates physical separation from the family.",
     "3rd House: Loss of courage or siblings moving far away; indicates highly secretive, isolated communications and writing.",
     "4th House: Detachment from hometown; settling permanently in foreign lands; creates a deeply spiritual domestic life.",
     "5th House: Detachment from conventional children or romance; possesses a deep, spiritual, and highly imaginative intellect.",
     "6th House: Vipareeta Raja Yoga. Crushes enemies, debts, and diseases through isolation, hidden strategies, or foreign help.",
     "7th House: Attracts a foreign spouse, or indicates physical separation, travel, and detachment within business partnerships.",
     "8th House: Vipareeta Raja Yoga. Profound occult mastery, escaping hidden traps successfully, and extreme spiritual depth.",
     "9th House: Foreign travel specifically for higher learning; possesses a highly spiritual but totally unorthodox ideology.",
     "10th House: Career in completely isolated places (hospitals, prisons) or working entirely for foreign corporations.",
     "11th House: Builds massive networks in foreign lands; high expenses directly linked to fulfilling worldly desires.",
     "12th House: Absolute Moksha. Deep spiritual liberation, profound inner peace in total isolation, and clearing all karmic debts."
    }
};

inline std::string get_lord_in_house_text(int lord_h, int placed_h) {
    if (lord_h >= 1 && lord_h <= 12 && placed_h >= 1 && placed_h <= 12) {
        return bhava_lord_matrix[lord_h][placed_h];
    }
    return "Neutral lordship placement.";
}

// =========================================================================
// PHASE 6: AUSPICIOUSNESS SCORING MATRICES
// =========================================================================

// RVS Default Bad Placements (0=Mesh, 1=Vri, 2=Mit, 3=Kar, 4=Sim, 5=Kan, 6=Tul, 7=Vrc, 8=Dha, 9=Mak, 10=Kum, 11=Mee)
// 1 means it is a BAD placement for that planet.
const int rvs_bad_placements[10][12] = {
    {0,0,0,0,0,0,0,0,0,0,0,0}, // 0: Lagna (unused)
    {0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0}, // 1: Surya (Vri, Tul, Mak, Kum)
    {1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0}, // 2: Chandra (Mes, Vrc, Mak, Kum)
    {0, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0}, // 3: Mangal (Vri, Mit, Kar, Tul, Kan)
    {1, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 1}, // 4: Budha (Mes, Kar, Vrc, Dha, Mee)
    {0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0}, // 5: Guru (Vri, Mit, Tul, Kan, Mak, Kum)
    {1, 0, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0}, // 6: Shukra (Mes, Mit, Kar, Sim, Kan, Vrc)
    {1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0}, // 7: Shani (Mes, Kar, Sim, Vrc)
    {1, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0}, // 8: Rahu (Mes, Kar, Vrc, Dha)
    {0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0}  // 9: Ketu (Vri, Mit, Tul, Kan, Mak, Kum)
};

// Natural Enemy Map (Does planet X consider planet Y an enemy?)
// 1 = Yes (Enemy), 0 = No (Neutral/Friend)
const int natural_enemies[10][10] = {
    {0,0,0,0,0,0,0,0,0,0}, // 0: Lagna
    {0, 0, 0, 0, 0, 0, 1, 1, 1, 1}, // 1: Surya hates Ve, Sa, Ra, Ke
    {0, 0, 0, 0, 0, 0, 0, 0, 1, 1}, // 2: Chandra hates Ra, Ke
    {0, 0, 0, 0, 1, 0, 0, 0, 1, 1}, // 3: Mangal hates Me, Ra, Ke
    {0, 0, 1, 0, 0, 0, 0, 0, 0, 0}, // 4: Budha hates Mo
    {0, 0, 0, 0, 1, 0, 1, 0, 0, 0}, // 5: Guru hates Me, Ve
    {0, 1, 1, 0, 0, 0, 0, 0, 0, 0}, // 6: Shukra hates Su, Mo
    {0, 1, 1, 1, 0, 0, 0, 0, 0, 0}, // 7: Shani hates Su, Mo, Ma
    {0, 1, 1, 1, 0, 0, 0, 0, 0, 0}, // 8: Rahu hates Su, Mo, Ma
    {0, 1, 1, 1, 0, 0, 0, 0, 0, 0}  // 9: Ketu hates Su, Mo, Ma
};

// =========================================================================
// GOCHARA PHALA MATRIX (ENGLISH TRANSIT PREDICTIONS FROM MOON SIGN)
// =========================================================================

inline std::string get_gochar_text(int p_idx, int from_mo) {
    if (p_idx == 5) { // GURU (Jupiter)
        switch(from_mo) {
            case 1: return "Transit in Janma Rasi: Indicates relocation, mental distress, friction with superiors or loved ones, and increased expenses.";
            case 2: return "Transit in 2nd House: Highly Auspicious. Brings family happiness, financial gains, great recognition, and wealth accumulation.";
            case 3: return "Transit in 3rd House: Indicates career obstacles, location changes or transfers, and minor disagreements with siblings.";
            case 4: return "Transit in 4th House: Brings domestic unrest, potential health issues for the mother, or worries regarding property and vehicles.";
            case 5: return "Transit in 5th House: Highly Auspicious. Excellent time for marriage, childbirth, buying property/vehicles, and massive business/speculative profits.";
            case 6: return "Transit in 6th House: Indicates health issues, debt pressures, or sudden fear/litigation from enemies.";
            case 7: return "Transit in 7th House: Highly Auspicious. Marriage for singles. Excellent business/career gains, health improvement, and marital bliss.";
            case 8: return "Transit in 8th House: Warns of illness, unexpected financial loss, fatigue during travel, and severe delays in undertakings.";
            case 9: return "Transit in 9th House: Highly Auspicious. Immense fortune, pilgrimages, foreign travels, and overall wealth expansion.";
            case 10: return "Transit in 10th House: Indicates sudden career changes, fear of job loss or unwanted transfers, and potential business struggles.";
            case 11: return "Transit in 11th House: Highly Auspicious. Fulfillment of all desires. Massive financial gains, promotions, and auspicious events at home.";
            case 12: return "Transit in 12th House: Indicates heavy expenses for auspicious events (marriage, house construction) and long-distance travels.";
        }
    }
    else if (p_idx == 7) { // SHANI (Saturn)
        switch(from_mo) {
            case 1: return "Janma Shani (Middle of Sade Sati): Severe mental anxiety, physical fatigue, heavy workload, and potential illness.";
            case 2: return "2nd House Shani (End of Sade Sati): Financial constraints, family disputes, and unexpected financial losses or harsh speech.";
            case 3: return "3rd House Shani: Highly Auspicious. Increased courage, destruction of enemies, new job or property, and massive success.";
            case 4: return "4th House Shani (Ardhashtama): Domestic unrest, mother's health issues, professional frustrations, and friction with relatives.";
            case 5: return "5th House Shani: Worries about children, investment delays, and misunderstandings or coldness in romance.";
            case 6: return "6th House Shani: Highly Auspicious. Absolute victory over court cases, enemies, and diseases. Debts clear up, financial recovery.";
            case 7: return "7th House Shani: Severe misunderstandings with spouse, business partner disputes, and heavy, fruitless travels.";
            case 8: return "8th House Shani (Ashtama): Sudden career obstacles, severe health issues, and unexpected hardships. Requires extreme caution.";
            case 9: return "9th House Shani: Father's health issues, detachment from religion/luck, and depression or delays in higher studies/travel.";
            case 10: return "10th House Shani: Immense workload in career. Friction with authorities, and a lack of recognition for extreme hard work.";
            case 11: return "11th House Shani: Highly Auspicious. Permanent financial gains in all areas, acquisition of new house/vehicle, and great social respect.";
            case 12: return "12th House Shani (Start of Sade Sati): Wasteful expenses, insomnia, eye/foot aches, hidden fears, and unexpected travels.";
        }
    }
    else if (p_idx == 8 || p_idx == 9) { // RAHU & KETU
        if (from_mo == 3 || from_mo == 6 || from_mo == 11) {
            return "Shadow Planet in Upachaya House: Highly favorable transit. Indicates sudden massive gains, foreign travels, and destruction of enemies.";
        } else {
            return "This shadow planet transit creates mental confusion, sudden changes, and karmic instability regarding the themes of this house.";
        }
    }
    
    // Default fallback for Sun, Moon, Mars, Mercury, Venus (Fast moving planets)
    if (from_mo == 3 || from_mo == 6 || from_mo == 10 || from_mo == 11) {
        return "Favorable transit from the Moon. Brings short-term success in endeavors, gains, and a positive environment.";
    } else {
        return "Standard transit results. May require routine effort or cause minor, short-term delays.";
    }
}