#include "types.h"
#include "ending.h"
#include "libc.h"

#include "mode.h"

/* Values the ending messages test and quote; field names are inferred from use. */
struct EndingMsgStats
{
    s32 total;      /* %p */
    s32 pointsAll;  /* %t */
    s32 points;
    u8 stagesDone;
    u8 stagesLeft;
    s32 overTarget;
    s32 hasBonus;
    s32 overTarget2;
    s32 gotAll;
    u8 unlocked; /* %c */
    s32 pointsToGo; /* %n */
    s32 bonusNoStages;
    s32 firstTime;
    s32 best;    /* %b */
    s32 max;     /* %m */
};

extern s32 active_monkey_id[];
extern s32 lbl_1_bss_D3350;
extern u8 lbl_1_bss_DDA38;
extern s32 lbl_1_bss_DDA3C[];
extern int return_5(void);

char *lbl_1_data_63D94[] = {
    "h/KONNDOHA SARANI",
    "h/UENOk/REBERUh/DE ASONNDENE",
    NULL,
};

char *lbl_1_data_63DD8[] = {
    "h/KOREDE k/KIMIh/MO",
    "h/ICHININNMAENO Ok/SARUh/SANNDAYO",
    NULL,
};

char *lbl_1_data_63E28[] = {
    "k/KIMIh/TTEk/SUGOIh/NE! KOREKARAMO",
    "k/MONNKI-BO-RUh/WO YOROSIKU!",
    NULL,
};

char *lbl_1_data_63EE0[] = {
    "h/KOKOMADE, k/PUREIh/DEKITAk/KIMIh/HAk/SUGOIh/YO!",
    "h/KOKOMADENOk/PUREIh/, OTUKARESAMADESITA!",
    "p/TSUGI/h/NIk/PUREIh/SURUTOKIHA, SAIGOMADE",
    "h/IKERUYOUNI, GANNBATTEMITENE!",
    NULL,
};

char *lbl_1_data_63F70[] = {
    "h/%pk/POINNTOh/WOk/GETTOh/SHITAYO!",
    "h/ZENBUDE%tk/POINNTOh/DANE!",
    "h/ATO%nk/POINNTOh/DE",
    "k/MINIGE-MUh/WOk/GETTOh/DEKIRUYO!",
    NULL,
};

char *lbl_1_data_63FF4[] = {
    "h/%pk/POINNTOh/WOk/GETTOh/SHITAYO!",
    "h/ZENNBUDE%tk/POINNTOh/DANE!",
    "k/POINNTOh/GATAKUSANNTAMATTAKARA",
    "h/SUKINAk/MINIGE-MUh/WOk/GETTOh/DEKIRUYO!",
    NULL,
};

char *lbl_1_data_64058[] = {
    "%pk/POINNTOh/WOk/GETTOh/SHITAYO!",
    "h/ZENBUDE%tk/POINNTOh/DANE!",
    "k/POINNTOh/GATAKUSANNTAMATTAKARA",
    "h/SAIGONOk/MINIGE-MUh/WOk/GETTOh/SHITENE!",
    NULL,
};

char *lbl_1_data_640BC[] = {
    "k/%pk/POINNTOh/WOk/GETTOh/SHITAYO!",
    "h/ZENNBUDE%tk/POINNTOh/DANE!",
    "h/ATO%nk/POINNTOh/DE",
    "h/k/KONNTEXINYU-h/p/KAISUUh/WOHUYASERUYO!",
    NULL,
};

char *lbl_1_data_64104[] = {
    "k/%pk/POINNTOh/WOk/GETTOh/SHITAYO!",
    "h/ZENBUDE%tk/POINNTOh/DANE!",
    "k/POINNTOh/GATAKUSANNTAMATTAKARA",
    "k/KONNTEXINYU-p/KAISUU/h/GA%cp/KAIsuu/h/NINATTAYO!",
    NULL,
};

char *lbl_1_data_6415C[] = {
    "k/%pk/POINNTOh/WOk/GETTOh/SHITAYO!",
    "h/ZENBUDE%tk/POINNTOh/DANE!",
    "h/KOREDEk/KONNTEXINYU-h/GAp/NANNKAI/h/DEMO",
    "h/DEKIRUYOUNINATTAYO!",
    NULL,
};

char *lbl_1_data_641F8[] = {
    "k/%pk/POINNTOh/WOk/GETTOh/SHITAYO!",
    "k/KIMIh/HA, SUBETENOMONOWOk/GETTOh/SHITEIRUYO!",
    "h/KOREKARAHA, 1k/PUREIh/DEk/GETTOh/DEKIRU",
    "k/PUREIPOINNTOh/KIROKUNIk/TYARENJIh/DAYO!",
    NULL,
};

char *lbl_1_data_64270[] = {
    "k/%pk/POINNTOh/WOk/GETTOh/SHITAYO!",
    "k/SAIKO-h/HA, %bk/POINNTOh/DANE!",
    "k/SAIKO-h/KIROKUWOKOERARERUYOUNI",
    "h/KOREKARAMO, GANBATTENE!",
    NULL,
};

char *lbl_1_data_642EC[] = {
    "k/%pk/POINNTOh/WOk/GETTOh/SHITAYO!",
    "h/KORETTE, k/SAIKO-h/KIROKUDAYO!",
    "k/YATTAh/NE! KOREKARAMO, SARANIUENO",
    "h/KIROKUWOMEZASITE, GANBATTENE!",
    NULL,
};

char *lbl_1_data_6436C[] = {
    "k/%pk/POINNTOh/WOk/GETTOh/SHITAYO!",
    "h/KOREYORIk/SUGOIh/KIROKUHANAIYO!",
    "k/KIMIh/HA, k/KANPEKIh/DANE!",
    "h/TYO-k/MIRAKURUh/NAk/PUREIh/DATTAYO!",
    NULL,
};

char *lbl_1_data_643E4[] = {
    "k/%pk/POINNTOh/WOk/GETTOh/SHITAYO!",
    "k/SAIKO-h/HA, %mk/POINNTOh/DANE!",
    "h/MOUITIDO, %mk/POINNTOh/DASERUKA",
    "k/TYARENJIh/SHITEMITENE!",
    NULL,
};

char *lbl_1_data_6442C[] = {
    "h/KONNDOHA MOTTO",
    "h/UENOk/REBERUh/WO MEZASHITENE",
    NULL,
};

char *lbl_1_data_6446C[] = {
    "h/ANATAMO KOREDE",
    "h/ICHININNMAENO Ok/SARUh/SANNNE",
    NULL,
};

char *lbl_1_data_644B4[] = {
    "h/ANATATTEk/SUGOIh/WA! KOREKARAMO",
    "k/PUREI h/SHIMAKUTTENE",
    NULL,
};

char *lbl_1_data_64580[] = {
    "h/KOKOMADEk/PUREIh/DEKITAANATAHA, k/ERAIh/WA!",
    "p/JIBUNN/h/DEp/JIBUNN/h/WOHOMETEAGETEMOIIKAMO!",
    "p/IMA/p/KAIsuu/h/HAAKIRAMETAKEDO, p/TSUGI/h/NOk/PUREIh/DEHA",
    "k/KURIAh/DEKIRUYOUNI, GANNBATTENE!",
    NULL,
};

char *lbl_1_data_64600[] = {
    "k/%pk/POINNTOh/WOk/GETTOh/SHITAWA!",
    "h/ZE~NBUDE%tk/POINNTOh/YO.",
    "h/ATO%nk/POINNTOh/DE",
    "k/MINIGE-MUh/GAk/GETTOh/DEKITYATTARISHITE.",
    NULL,
};

char *lbl_1_data_64668[] = {
    "k/%pk/POINNTOh/WOk/GETTOh/SHITAWA!",
    "h/ZE~NBUDE%tk/POINNTOh/YO.",
    "h/TAKKUSA~NNk/POINNTOh/GAATUMATTAKARA",
    "h/SUKINAk/MINIGE-MUh/WOk/GETTOh/DEKIRUWAYO!",
    NULL,
};

char *lbl_1_data_646AC[] = {
    "k/%pk/POINNTOh/WOk/GETTOh/SHITAWA!",
    "h/ZE~NBUDE%tk/POINNTOh/YO.",
    "h/TAKKUSA~NNk/POINNTOh/GAATUMATTAKARA",
    "h/SAIGONOk/MINIGE-MUh/MOk/GETTOh/DEKIRUWAYO!",
    NULL,
};

char *lbl_1_data_646EC[] = {
    "k/%pk/POINNTOh/WOk/GETTOh/SHITAWA!",
    "h/ZE~NBUDE%tk/POINNTOh/YO.",
    "h/ATO%nk/POINNTOh/DE",
    "k/KONNTEXINYU-p/KAISUU/h/WOHUYASERUWAYO!",
    NULL,
};

char *lbl_1_data_64734[] = {
    "k/%pk/POINNTOh/WOk/GETTOh/SHITAWA!",
    "h/ZE~NBUDE%tk/POINNTOh/YO.",
    "h/TAKKUSA~NNk/POINNTOh/GAATUMATTAKARA",
    "k/KONNTEXINYU-p/KAISUU/h/GA%cp/KAIsuu/h/NINATTAWA!",
    NULL,
};

char *lbl_1_data_64760[] = {
    "k/%pk/POINNTOh/WOk/GETTOh/SHITAWA!",
    "h/ZE~NBUDE%tk/POINNTOh/YO.",
    "h/KOREDEk/KONNTEXINYU-h/GAp/NANNKAI/h/DEMO",
    "h/DEKIRUYOUNINATTAWAYO!",
    NULL,
};

char *lbl_1_data_647EC[] = {
    "k/%pk/POINNTOh/WOk/GETTOh/SHITAWA!",
    "h/ANATANIAGERARERUMONOHA, MOUNAIWA!",
    "h/KOREKARAHA, 1k/PUREIh/DEk/PUREIPOINNTOh/WO",
    "h/DOREDAKETORERUKANI, TYOUSENNYO!",
    NULL,
};

char *lbl_1_data_64868[] = {
    "k/%pk/POINNTOh/WOk/GETTOh/SHITAWA!",
    "k/SAIKO-h/HA, %bk/POINNTOh/YO!",
    "k/SAIKO-h/KIROKUWONURIKAERARERUYOUNI",
    "h/KOREKARAMO, ASOBITUDUKETENE!",
    NULL,
};

char *lbl_1_data_648DC[] = {
    "k/%pk/POINNTOh/WOk/GETTOh/SHITAWA!",
    "h/KOREHA, k/SAIKO-h/KIROKUDAWA!",
    "k/SUGGO~I! h/KOREKARAMO",
    "h/UENOKIROKUWOMEZASHITE, GANBATTENE!",
    NULL,
};

char *lbl_1_data_64968[] = {
    "k/%pk/POINNTOh/WOk/GETTOh/SHITAWA!",
    "h/KOREIJYOUNOKIROKUHA, DENAIWAYO!",
    "h/ANATATTE, k/PA-FEKUTOh/NE!",
    "h/MOU, k/SUTEKIp/./k/TOKIMEKIp/./k/DA~ISUKIh/YO!",
    NULL,
};

char *lbl_1_data_649E0[] = {
    "k/%pk/POINNTOh/WOk/GETTOh/SHITAWA!",
    "h/KIROKUHA,%mk/POINNTOh/YO!",
    "h/%mk/POINNTOh/TOIUk/SAIKO-h/KIROKUWO",
    "h/MATADASERUKA, TYOUSENNSITENE!",
    NULL,
};

char *lbl_1_data_64A24[] = {
    "h/MOTTO UENOk/REBERUh/DE",
    "h/ASOBITAIDEk/CHU",
    NULL,
};

char *lbl_1_data_64A68[] = {
    "h/KOREDE RIPPANA Ok/SARUh/SANNNO",
    "h/NAKAMAIRIDEk/CHU",
    NULL,
};

char *lbl_1_data_64AB0[] = {
    "k/SUGOIh/DEk/CHU! h/KOREKARAMO",
    "h/ZUTTO ASONNDEHOSIIDEk/CHU",
    NULL,
};

char *lbl_1_data_64B58[] = {
    "h/KONOk/SUTE-JIh/MADEk/PUREIh/DEKIRUNANNTE",
    "k/SUGOIh/DEk/TYU! ODOROKIh/DEk/TYU!",
    "h/TOTTEMOk/MUZUKAh/TIIDEk/TYUh/KEDO",
    "h/TUGIHAk/SAIGOh/MADEITTEHOTIIDEk/TYU!",
    NULL,
};

char *lbl_1_data_64BD0[] = {
    "h/%pk/POINNTO, GETTOh/DEk/TYU!",
    "h/ZENBUDE%tk/POINNTOh/DEk/TYU!",
    "h/ATO%nk/POINNTOh/DE",
    "k/MINIGE-MUGETTOh/DEk/TYU! BABU~!",
    NULL,
};

char *lbl_1_data_64C44[] = {
    "h/%pk/POINNTO, GETTOh/DEk/TYU!",
    "h/ZENBUDE%tk/POINNTOh/DEk/TYU!",
    "k/POINNTOh/WOTAKUSANNk/GETTOh/TITADEk/TYUh/KARA",
    "h/SUKINAk/MINIGE-MUh/WOk/GETTOh/DEKIRUDEk/TYU!",
    NULL,
};

char *lbl_1_data_64CA8[] = {
    "h/%pk/POINNTO,GETTOh/DEk/TYU!",
    "h/ZENBUDE%tk/POINNTOh/DEk/TYU!",
    "k/POINNTOh/WOTAKUSANNk/GETTOh/TITADEk/TYUh/KARA",
    "k/MINIGE-MUh/WOZENNBUk/GETTOh/DEKIRUDEk/TYU!",
    NULL,
};

char *lbl_1_data_64CE0[] = {
    "h/%pk/POINNTO,GETTOh/DEk/TYU!",
    "h/ZENBUDE%tk/POINNTOh/DEk/TYU!",
    "h/ATO%nk/POINNTOh/DE",
    "k/KONNTEXINYU-h/WOHUYASERUDEk/TYU!",
    NULL,
};

char *lbl_1_data_64D20[] = {
    "h/%pk/POINNTO,GETTOh/DEk/TYU!",
    "h/ZENBUDE%tk/POINNTOh/DEk/TYU!",
    "k/POINNTOh/WOTAKUSANNk/GETTOh/TITADEk/TYUh/KARA",
    "k/KONNTEXINYU-h/GA%ch/KAININATTADEk/TYU!",
    NULL,
};

char *lbl_1_data_64D78[] = {
    "h/%pk/POINNTO,GETTOh/DEk/TYU!",
    "h/ZENBUDE%tk/POINNTOh/DEk/TYU!",
    "h/NANNKAIDEMOk/KONNTEXINYU-h/GA",
    "h/DEKIRUYOUNINATTADEk/TYU! BABU~!",
    NULL,
};

char *lbl_1_data_64DE8[] = {
    "h/%pk/POINNTO, GETTOh/DEk/TYU!",
    "h/MOU, k/PUREZENNTOh/SURUMONOHANAIDEk/TYU!",
    "h/KOREKARAHA, 1k/PUREIh/DEk/PUREIPOINNTOh/WO",
    "h/IKUTUk/GETTOh/SURUKANIk/TYARENJIh/DEk/TYU!",
    NULL,
};

char *lbl_1_data_64E60[] = {
    "h/%pk/POINNTO,GETTOh/DEk/TYU!",
    "k/SAIKO-h/HA%bk/POINNTOh/DEk/TYU!",
    "k/SAIKO-h/KIROKU, MEZATITE",
    "h/KOREKARAMO, k/PUREIh/SURUDEk/TYU!",
    NULL,
};

char *lbl_1_data_64EE0[] = {
    "h/%pk/POINNTO, GETTOh/DEk/TYU!",
    "h/KOREHA, k/SAIKO-h/KIROKUDEk/TYU!",
    "h/YATTADEk/TYU! h/URETIIDEk/TYU!",
    "k/SUGOIh/DEk/TYU! UKIUKIh/DEk/TYU!",
    NULL,
};

char *lbl_1_data_64F54[] = {
    "h/%pk/POINNTO, GETTOh/DEk/TYU!",
    "k/PA-FEKUTOh/NAKIROKUDEk/TYU!",
    "h/KOREYORIk/SUGOIh/KIROKUHA",
    "h/MOUNAIDEk/TYU! ODOROKIh/DEk/TYU!",
    NULL,
};

char *lbl_1_data_64FDC[] = {
    "h/%pk/POINNTO, GETTOh/DEk/TYU!",
    "k/SAIKO-h/HA,%mk/POINNTOh/DEk/TYU!",
    "h/%mk/POINNTOh/WOMATAk/GETTOh/DEKIRUKA",
    "k/TYARENNJIh/TITEHOSIIDEk/TYU! BABU~!",
    NULL,
};

char *lbl_1_data_65030[] = {
    "k/SAIKYO~NOSARU MEZASHITE MOTTO",
    "k/UENO REBERUDE ASONDEKURE~!",
    NULL,
};

char *lbl_1_data_6507C[] = {
    "k/SUGOIJYANE~KA! ODOROKIDAZE!",
    "k/DAXA~KEDO UEHA MADA ARUZE!",
    NULL,
};

char *lbl_1_data_650C4[] = {
    "k/UHOUHO UHOHO~I! YATTA~!",
    "k/KOREDE SAIKYO~NOSARU DAZE~!",
    NULL,
};

char *lbl_1_data_65160[] = {
    "k/KONOSUTE~JIMADE KORETA OMAEHA",
    "k/KANARINO UDEMAEDA~XTU! UHOUHO!",
    "k/TUGINI PUREISURUTOKIHA AKIRAMEZUNI",
    "k/ZENNBU KURIA DEKIRUYOUNI GANBARE!",
    NULL,
};

char *lbl_1_data_651EC[] = {
    "k/%pk/POINNTO GETTOXO~XTU!",
    "k/ZENBUDE %tk/POINNTOXO~XTU!",
    "k/ATO %nk/POINNTO DE",
    "k/MINIGEXE~MU GETTOXO~XTU! UHOHO!",
    NULL,
};

char *lbl_1_data_65240[] = {
    "k/%pk/POINNTO GETTOXO~XTU!",
    "k/ZENBUDE %tk/POINNTOXO~XTU!",
    "k/POINNTO TAKUSANN! DAXAKA~RA",
    "k/MINIGE~MU GETTOXO~XTU! DAXA~!",
    NULL,
};

char *lbl_1_data_65278[] = {
    "k/%pk/POINNTO GETTOXO~XTU!",
    "k/ZENBUDE %tk/POINNTOXO~XTU!",
    "k/POINNTO TAKUSANN! DAXAKA~RA",
    "k/MINIGEXE~MU ZE~NNBU GETTOXO~XTU!",
    NULL,
};

char *lbl_1_data_652AC[] = {
    "k/%pk/POINNTO GETTOXO~XTU!",
    "k/ZENBUDE %tk/POINNTOXO~XTU!",
    "k/ATO %nk/POINNTO DE",
    "k/KONNTEXINYU~ HUE~RU! UHOUHO!",
    NULL,
};

char *lbl_1_data_652E8[] = {
    "k/%pk/POINNTO GETTOXO~XTU!",
    "k/ZENBUDE %tk/POINNTOXO~XTU!",
    "k/POINNTO TAKUSANN! DAXAKA~RA",
    "k/KONNTEXINYU~ %ck/ KAINI HUETA~XTU!",
    NULL,
};

char *lbl_1_data_65340[] = {
    "k/%pk/POINNTO GETTOXO~XTU!",
    "k/ZENBUDE %tk/POINNTOXO~XTU!",
    "k/DAXAKA~RA KONNTEXINYU~ NANNDOMO",
    "k/DEKIRUYOUNI NATTA~! NATTA~!",
    NULL,
};

char *lbl_1_data_653BC[] = {
    "k/%pk/POINNTO GETTOXO~XTU!",
    "k/MOU OMAENI YARERUMONO NAI!",
    "k/KOREKARAHA 1PUREIDE PUREIPOINNTO",
    "k/NANNTENN GETTODEKIRUKA TYARENNJI!",
    NULL,
};

char *lbl_1_data_65434[] = {
    "k/%pk/POINNTO GETTOXO~XTU!",
    "k/SAIKO~%bk/POINNTO! ZANNNENN!",
    "k/KOREKARAMO SAIKO~KIROKU MEZASITE",
    "k/PUREI SURUNO DA~XTU! UHOHO~I!",
    NULL,
};

char *lbl_1_data_654A8[] = {
    "k/%pk/POINNTO GETTOXO~XTU!",
    "k/SAIKO~KIROKU DETTA~! DETTA~!",
    "k/UHO UHOHO! UHOUHO UHOHO~I!",
    "k/YATTA~ YATTA~! YATTA~GO~NN!",
    NULL,
};

char *lbl_1_data_65520[] = {
    "k/%pk/POINNTO GETTOXO~XTU!",
    "k/KO KOREHA ODOROKINO DA~IKIROKU!",
    "k/KOREIJYO~ SUGOI KIROKU DENAI!",
    "k/OMAE TENNSA~I SUGOIZOXO~XTU!",
    NULL,
};

char *lbl_1_data_655A4[] = {
    "k/%pk/POINNTO GETTOXO~XTU!",
    "k/SAIKO~%mk/POINNTO! ZANNNENN",
    "k/%mk/POINNTO MATA DASERUYO~NI",
    "k/GANNBARE~XTU! 1p/./k/2p/./k/3p/./k/DAXA~XTU!",
    NULL,
};

char **lbl_1_data_655B8[15] = {
    lbl_1_data_63D94,
    lbl_1_data_63DD8,
    lbl_1_data_63E28,
    lbl_1_data_63EE0,
    lbl_1_data_63F70,
    lbl_1_data_63FF4,
    lbl_1_data_64058,
    lbl_1_data_640BC,
    lbl_1_data_64104,
    lbl_1_data_6415C,
    lbl_1_data_641F8,
    lbl_1_data_64270,
    lbl_1_data_642EC,
    lbl_1_data_6436C,
    lbl_1_data_643E4,
};

char **lbl_1_data_655F4[15] = {
    lbl_1_data_6442C,
    lbl_1_data_6446C,
    lbl_1_data_644B4,
    lbl_1_data_64580,
    lbl_1_data_64600,
    lbl_1_data_64668,
    lbl_1_data_646AC,
    lbl_1_data_646EC,
    lbl_1_data_64734,
    lbl_1_data_64760,
    lbl_1_data_647EC,
    lbl_1_data_64868,
    lbl_1_data_648DC,
    lbl_1_data_64968,
    lbl_1_data_649E0,
};

char **lbl_1_data_65630[15] = {
    lbl_1_data_64A24,
    lbl_1_data_64A68,
    lbl_1_data_64AB0,
    lbl_1_data_64B58,
    lbl_1_data_64BD0,
    lbl_1_data_64C44,
    lbl_1_data_64CA8,
    lbl_1_data_64CE0,
    lbl_1_data_64D20,
    lbl_1_data_64D78,
    lbl_1_data_64DE8,
    lbl_1_data_64E60,
    lbl_1_data_64EE0,
    lbl_1_data_64F54,
    lbl_1_data_64FDC,
};

char **lbl_1_data_6566C[15] = {
    lbl_1_data_65030,
    lbl_1_data_6507C,
    lbl_1_data_650C4,
    lbl_1_data_65160,
    lbl_1_data_651EC,
    lbl_1_data_65240,
    lbl_1_data_65278,
    lbl_1_data_652AC,
    lbl_1_data_652E8,
    lbl_1_data_65340,
    lbl_1_data_653BC,
    lbl_1_data_65434,
    lbl_1_data_654A8,
    lbl_1_data_65520,
    lbl_1_data_655A4,
};

char ***lbl_1_data_656A8[4] = {
    lbl_1_data_655B8,
    lbl_1_data_655F4,
    lbl_1_data_65630,
    lbl_1_data_6566C,
};

void fn_1_10913C(struct EndingMsgStats *out);
char **fn_1_109328(struct EndingMsgStats *in, char ***out);
void fn_1_109530(struct EndingMsgStats *in, struct EndingMsgLines *lines, char *buf, char **msg);

void fn_1_1090E0(struct EndingMsgLines *lines, char *buf)
{
    struct EndingMsgStats stats;

    fn_1_10913C(&stats);
    fn_1_109530(&stats, lines, buf, fn_1_109328(&stats, NULL));
}

void fn_1_10913C(struct EndingMsgStats *out)
{
    struct EndingMsgStats stats;

    memset(&stats, 0, sizeof(stats));
    stats.stagesDone = 0;
    stats.total = 0;
    stats.pointsAll = 0;
    stats.points = 0;
    stats.stagesLeft = 3 - stats.stagesDone;
    stats.overTarget = stats.points > 2500;
    stats.hasBonus = lbl_1_bss_DDA38 & 8;
    stats.overTarget2 = stats.points > 2500;
    stats.unlocked = return_5();
    {
        s32 p = stats.points;

        while (p > 2500)
        {
            p -= 2500;
            stats.unlocked++;
        }
    }
    stats.gotAll = stats.unlocked > (u8)9;
    stats.pointsToGo = 2500 - stats.points;
    stats.bonusNoStages = (stats.hasBonus != 0 && stats.stagesLeft == 0);
    stats.firstTime = !(lbl_1_bss_D3350 & 1);
    stats.best = 0;
    stats.max = lbl_1_bss_DDA3C[modeCtrl.difficulty];
    *out = stats;
}

char **fn_1_109328(struct EndingMsgStats *in, char ***out)
{
    struct EndingMsgStats stats = *in;
    char **msg;
    char ***table = lbl_1_data_656A8[active_monkey_id[modeCtrl.currPlayer]];

    if (modeCtrl.playerCount > 1)
    {
        switch (modeCtrl.difficulty)
        {
        case 0:
            msg = table[0];
            break;
        case 1:
            msg = table[1];
            break;
        case 2:
        default:
            msg = table[2];
            break;
        }
    }
    else if (stats.stagesLeft != 0)
    {
        if (!stats.overTarget)
            msg = table[4];
        else if (stats.stagesLeft > 1)
            msg = table[5];
        else
            msg = table[6];
    }
    else if (!stats.hasBonus)
    {
        if (!stats.overTarget2)
            msg = table[7];
        else if (!stats.gotAll)
            msg = table[8];
        else
            msg = table[9];
    }
    else if (stats.firstTime)
    {
        msg = table[10];
        lbl_1_bss_D3350 |= 1;
    }
    else if (stats.total >= stats.best)
    {
        if (stats.best == stats.max)
            msg = table[13];
        else
            msg = table[12];
    }
    else
    {
        if (stats.best == stats.max)
            msg = table[14];
        else
            msg = table[11];
    }
    if (out != NULL)
        *out = msg;
    return msg;
}

void fn_1_109530(struct EndingMsgStats *in, struct EndingMsgLines *lines, char *buf, char **msg)
{
    struct EndingMsgLines result;
    struct EndingMsgStats stats = *in;
    char **line;
    u8 *src;
    u8 *dst;

    memset(&result, 0, sizeof(result));
    line = msg;
    dst = (u8 *)buf;
    while (*line != NULL)
    {
        result.lines[result.count] = (char *)dst;
        src = (u8 *)*line;
        while (*src != 0)
        {
            switch (*src)
            {
            case '%':
                src++;
                switch (*src)
                {
                case 'p':
                    sprintf((char *)dst, "c/0xff0000/h/%dc/0x000000/h/", stats.total);
                    break;
                case 't':
                    sprintf((char *)dst, "c/0xff0000/h/%dc/0x000000/h/", stats.pointsAll);
                    break;
                case 'n':
                    sprintf((char *)dst, "c/0xff0000/h/%dc/0x000000/h/", stats.pointsToGo);
                    break;
                case 'c':
                    sprintf((char *)dst, "c/0xff0000/h/%dc/0x000000/h/", stats.unlocked);
                    break;
                case 'b':
                    sprintf((char *)dst, "c/0xff0000/h/%dc/0x000000/h/", stats.best);
                    break;
                case 'm':
                    sprintf((char *)dst, "c/0xff0000/h/%dc/0x000000/h/", stats.max);
                    break;
                default:
                    *dst = 0;
                    break;
                }
                src++;
                while (*dst != 0)
                    dst++;
                break;
            default:
                *dst = *src;
                src++;
                dst++;
                break;
            }
        }
        *dst = 0;
        dst++;
        line++;
        result.count++;
    }
    *lines = result;
}
