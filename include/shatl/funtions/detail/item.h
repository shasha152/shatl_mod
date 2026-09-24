#pragma once

#include "shatl/il2cpp/il2cpp.h"
namespace tl {
namespace func {

struct item_ex {
    int width;
    int height;
    il2cpp::string *_nameOverride;
    bool questItem;
    bool flame;
    bool mech;
    int tileWand;
    bool wornArmor;
    int tooltipContext;
    char dye;
    int fishingPole;
    int bait;
    short makeNPC;
    bool expertOnly;
    bool expert;
    bool isAShopItem;
    short hairDye;
    char paint;
    char paintCoating;
    int type;
    bool favorited;
    char holdStyle;
    char useStyle;
    bool channel;
    bool accessory;
    int useAnimation;
    int useTime;
    int stack;
    int maxStack;
    int pick;
    int axe;
    int hammer;
    int tileBoost;
    int createTile;
    int createWall;
    int placeStyle;
    int damage;
    float knockBack;
    int healLife;
    int healMana;
    bool potion;
    bool consumable;
    bool autoReuse;
    bool useTurn;
    il2cpp::color color;
    int alpha;
    short glowMask;
    float scale;
    void *UseSound;
    float useSoundPitch;
    int defense;
    int headSlot;
    int bodySlot;
    int legSlot;
    char handOnSlot;
    char handOffSlot;
    char backSlot;
    char frontSlot;
    char shoeSlot;
    char waistSlot;
    char wingSlot;
    char shieldSlot;
    char neckSlot;
    char faceSlot;
    char balloonSlot;
    char beardSlot;
    char voiceSlot;
    int stringColor;
    void *ToolTip;
    il2cpp::string *BestiaryNotes;
    int rare;
    void *Variantk__BackingField;
    int shoot;
    float shootSpeed;
    int ammo;
    bool notAmmo;
    int useAmmo;
    int lifeRegen;
    int manaIncrease;
    bool buyOnce;
    int mana;
    bool noUseGraphic;
    bool noMelee;
    int value;
    bool buy;
    bool reforge;
    bool social;
    bool vanity;
    bool material;
    bool noWet;
    int buffType;
    int buffTime;
    int mountType;
    bool cartTrack;
    bool uniqueStack;
    int shopSpecialCurrency;
    void *shopCustomPrice;
    bool shootsEveryUse;
    bool chlorophyteExtractinatorConsumable;
    bool DD2Summon;
    int crit;
    int armorPenetration;
    int bonusTagDamage;
    char prefix;
    bool melee;
    bool magic;
    bool ranged;
    bool summon;
    bool sentry;
    int reuseDelay;
    bool newAndShiny;
    bool hasVanityEffects;
};

struct item : il2cpp::object<item>, item_ex {
    void change_item(int type) noexcept { call<void>("ChangeItemType", type); }
};
} // namespace func
} // namespace tl