/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *   File:        playerclasses.inc
 *   Description: Provides functions for managing classes.
 *   Author:      Richard Helgeby, Greyscale
 *
 * ============================================================================
 */

/*
    Ideas for immunity modes for humans:
    - Zombies have to stab x times to infect.
    - Zombies have to hurt humans so they loose hp. When the hp reach zero (or
      below) they turn into zombies.
    - Fully imune to all damage. Can't take or give damage. Sould only be used
      on admin classes.

    TODO: Admin command for overriding class values;
          zr_class_ovverride <classname> <key> <value> [value_is_multiplier]
          Either set absolute values or use it as a multiplier, if possible.
          "classname" should support "all" to set a spesific value on all
          classes.
          
          How overrides should work:
          When a player select a class, its attributes are cached for each
          player. On infection or spawn, the plugin will use the cached
          values.
          
          When we override, the real class attributes are changed and then
          copied to the player caches (with matching team IDs).
          
          There is one problem, the in-game knockback multiplier menu. It
          depends on the original class knockback values. A solution might be
          to have another array of class data, which is never canged.
    
    TODO: Make a solution with default valueas for each class team, without
          using CVARs.
          
          SOLVED: Added boolean team_default class attribute. Default values
          will be used from the first class with this value set.
    
    TODO: Make class attributes for for changing model render mode and colors.
    
    TODO: Make class attributes for fancy effects, like a glow (if possible).
    
    TODO: Make alpha settings also apply to weapons (we could use code from
          zombie riot in this case).
    
    TODO: Make immunity settings suitable for both teams, or certain zombie-
          only/human-only settings.
*/

/**
 * Total number of classes that can be stored in each cache. A total of 32
 * classes should be enough. Too many classes will comfuse players.
 */
#define ZR_CLASS_MAX 32

/**
 * @section Class cache types. Specifies what data array to use.
 */
#define ZR_CLASS_CACHE_ORIGINAL 0   /** Points ClassData array. A cache that is never changed after loading. */
#define ZR_CLASS_CACHE_MODIFIED 1   /** Points to ClassDataCache array. Modified by admins or overrides. */
#define ZR_CLASS_CACHE_PLAYER   2   /** Points to ClassPlayerCache array. Current player attributes. */
/**
 * @endsection
 */

/**
 * Number of available class teams.
 */
#define ZR_CLASS_TEAMCOUNT      3

/**
 * @section Available class teams. The admin team is optional and not required
 * in class configs.
 */
#define ZR_CLASS_TEAM_ZOMBIES   0
#define ZR_CLASS_TEAM_HUMANS    1
#define ZR_CLASS_TEAM_ADMINS    2   /** Note: Will set you in a special mode where you don't really participates in the game, but just walk around. */
/**
 * @endsection
 */

/**
 * @section Damage immunity modes. The mode effects will vary depending on the
 * class team.
 */
#define ZR_CLASS_IMMUNITY_DISABLED  0   /** No immunity. */
#define ZR_CLASS_IMMUNITY_CONSTANT  1   /** Always imune. Should only be used in special cases like on admin classes. */
#define ZR_CLASS_IMMUNITY_TIMED     2   /** Imune to damage for n seconds. The time is specified in a class as immunity amount. */
/**
 * @endsection
 */

/**
 * @section Overall default class settings. Since this is a zombie plugin the
 * default values represent a zombie.
 */
#define ZR_CLASS_DEFAULT_ENABLED                true
#define ZR_CLASS_DEFAULT_TEAM                   ZR_CLASS_TEAM_ZOMBIES
#define ZR_CLASS_DEFAULT_TEAM_DEFAULT           true
#define ZR_CLASS_DEFAULT_NAME                   "classic"
#define ZR_CLASS_DEFAULT_DESCRIPTION            "Need brains!!! Arrrrggghh!"
#define ZR_CLASS_DEFAULT_MODEL_PATH             "models/player/zh/zh_zombie003.mdl"
#define ZR_CLASS_DEFAULT_ALPHA_INITIAL          255
#define ZR_CLASS_DEFAULT_ALPHA_DAMAGED          255
#define ZR_CLASS_DEFAULT_ALPHA_DAMAGE           0
#define ZR_CLASS_DEFAULT_OVERLAY_PATH           "overlays/zr/zvision"
#define ZR_CLASS_DEFAULT_NVGS                   true
#define ZR_CLASS_DEFAULT_FOV                    90
#define ZR_CLASS_DEFAULT_NAPALM_TIME            10.0
#define ZR_CLASS_DEFAULT_IMMUNITY_MODE          ZR_CLASS_IMMUNITY_DISABLED
#define ZR_CLASS_DEFAULT_IMMUNITY_AMOUNT        0.0
#define ZR_CLASS_DEFAULT_NO_FALL_DAMAGE         true
#define ZR_CLASS_DEFAULT_HEALTH                 6000
#define ZR_CLASS_DEFAULT_HEALTH_REGEN_INTERVAL  0.0
#define ZR_CLASS_DEFAULT_HEALTH_REGEN_AMOUNT    2
#define ZR_CLASS_DEFAULT_HEALTH_INFECT_GAIN     800
#define ZR_CLASS_DEFAULT_KILL_BONUS             2
#define ZR_CLASS_DEFAULT_SPEED                  360.0
#define ZR_CLASS_DEFAULT_KNOCKBACK              2.0
#define ZR_CLASS_DEFAULT_JUMP_HEIGHT            10.0
#define ZR_CLASS_DEFAULT_JUMP_DISTANCE          0.2
/**
 * @endsection
 */

/**
 * @section Error flags for invalid class attributes.
 */
#define ZR_CLASS_ATTRIB_ERR_OK                      0
#define ZR_CLASS_ATTRIB_ERR_NAME                    1
#define ZR_CLASS_ATTRIB_ERR_DESCRIPTION             2
#define ZR_CLASS_ATTRIB_ERR_MODEL_PATH              4
#define ZR_CLASS_ATTRIB_ERR_ALPHA_INITIAL           8
#define ZR_CLASS_ATTRIB_ERR_ALPHA_DAMAGED           16
#define ZR_CLASS_ATTRIB_ERR_ALPHA_DAMAGE            32
#define ZR_CLASS_ATTRIB_ERR_OVERLAY_PATH            64
#define ZR_CLASS_ATTRIB_ERR_FOV                     128
#define ZR_CLASS_ATTRIB_ERR_NAPALM_TIME             256
#define ZR_CLASS_ATTRIB_ERR_IMMUNITY_MODE           512
#define ZR_CLASS_ATTRIB_ERR_IMMUNITY_AMOUNT         1024
#define ZR_CLASS_ATTRIB_ERR_HEALTH_REGEN_INTERVAL   2048
#define ZR_CLASS_ATTRIB_ERR_HEALTH_REGEN_AMOUNT     4096
#define ZR_CLASS_ATTRIB_ERR_INFECT_GAIN             8192
#define ZR_CLASS_ATTRIB_ERR_KILL_BONUS              16384
#define ZR_CLASS_ATTRIB_ERR_SPEED                   32768
#define ZR_CLASS_ATTRIB_ERR_KNOCKBACK               65536
#define ZR_CLASS_ATTRIB_ERR_JUMP_HEIGHT             131072
#define ZR_CLASS_ATTRIB_ERR_JUMP_DISTANCE           262144
/**
 * @endsection
 */


/**
 * Generic player attributes.
 */
enum ClassAttributes
{
    /* General */
    bool:class_enabled,
    class_team,
    bool:class_team_default,
    
    String:class_name[64],
    String:class_description[256],
    
    /* Model */
    String:class_model_path[256],
    class_alpha_initial,
    class_alpha_damaged,
    class_alpha_damage,
    
    /* Hud */
    String:class_overlay_path[256],
    bool:class_nvgs,
    class_fov,
    
    /* Effects */
    Float:class_napalm_time,
    
    /* Player behaviour */
    class_immunity_mode,
    Float:class_immunity_amount,
    bool:class_no_fall_damage,
    
    class_health,
    Float:class_health_regen_interval,
    class_health_regen_amount,
    class_health_infect_gain,
    class_kill_bonus,
    
    Float:class_speed,
    Float:class_knockback,
    Float:class_jump_height,
    Float:class_jump_distance
}

new Handle:kvClassData;

/**
 * The original class data. This array only changed when class data is loaded.
 * ZR_CLASS_CACHE_ORIGINAL is the cache type to this array.
 */
new ClassData[ZR_CLASS_MAX][ClassAttributes];

/**
 * The class thata cache that can be modified. ZR_CLASS_CACHE_MODIFIED is the
 * cache type to this array.
 */
new ClassDataCache[ZR_CLASS_MAX][ClassAttributes];

/**
 * Cache for player attributes. Makes it possible for one or more players to
 * have custom attributes. ZR_CLASS_CACHE_PLAYER is the cache type to this
 * array.
 */
new ClassPlayerCache[MAXPLAYERS + 1][ClassAttributes];

/**
 * Number of classes loaded.
 */
new ClassCount;

/**
 * Stores what class that the player have selected, for each team.
 */
new ClassSelected[MAXPLAYERS + 1][ZR_CLASS_TEAMCOUNT];

#include "zr/playerclasses/filtertools"
#include "zr/playerclasses/attributes"
#include "zr/playerclasses/apply"
#include "zr/playerclasses/clientoverlays"
#include "zr/playerclasses/clientalpha"
#include "zr/playerclasses/healthregen"
#include "zr/playerclasses/classevents"

/**
 * Loads class attributes from playerclasses.txt into the ClassData array. If
 * any error occour the plugin load will fail.
 *
 * @param classfile     Optional. Specifies what file to read from. Valves key/
 *                      values format. The path is relative to the sourcemod
 *                      folder.
 */
ClassLoad(const String:classfile[256] = "configs/zr/playerclasses.txt")
{
    // Make sure kvClassData is ready to use.
    if (kvClassData != INVALID_HANDLE)
    {
        CloseHandle(kvClassData);
    }
    kvClassData = CreateKeyValues("classes");
    
    // Try to load the class configuration file.
    decl String:path[PLATFORM_MAX_PATH];
    BuildPath(Path_SM, path, sizeof(path), classfile);
    
    if (!FileToKeyValues(kvClassData, path))
    {
        SetFailState("Class data (\"%s\") missing from server.", path);
    }
    
    // Try to find the first class.
    KvRewind(kvClassData);
    if (!KvGotoFirstSubKey(kvClassData))
    {
        SetFailState("Cannot find any classes in \"%s\".", path);
    }
    
    decl String:name[64];
    decl String:description[256];
    decl String:model_path[256];
    decl String:overlay_path[256];
    
    ClassCount = 0;
    new ClassErrorFlags;
    
    // Loop through all classes and store attributes in the ClassData array.
    do
    {
        if (ClassCount > ZR_CLASS_MAX)
        {
            // Maximum classes reached. Write a warning and exit the loop.
            if (LogFlagCheck(LOG_CORE_EVENTS, LOG_MODULE_CLASSES))
            {
                ZR_LogMessageFormatted(-1, "classes", "loading classes", "Warning: Maximum classes reached (%d). The rest is skipped.", _, ZR_CLASS_MAX + 1);
            }
            
            break;
        }
        
        /* General */
        ClassData[ClassCount][class_enabled] = bool:KvGetNum(kvClassData, "enabled", ZR_CLASS_DEFAULT_ENABLED);
        ClassData[ClassCount][class_team] = KvGetNum(kvClassData, "team", ZR_CLASS_DEFAULT_TEAM);
        ClassData[ClassCount][class_team_default] = bool:KvGetNum(kvClassData, "team_default", ZR_CLASS_DEFAULT_TEAM_DEFAULT);
        
        KvGetString(kvClassData, "name", name, sizeof(name), ZR_CLASS_DEFAULT_NAME);
        strcopy(ClassData[ClassCount][class_name], 64, name);
        
        KvGetString(kvClassData, "description", description, sizeof(description), ZR_CLASS_DEFAULT_DESCRIPTION);
        strcopy(ClassData[ClassCount][class_description], 256, description);
        
        
        /* Model */
        KvGetString(kvClassData, "model_path", model_path, sizeof(model_path), ZR_CLASS_DEFAULT_MODEL_PATH);
        strcopy(ClassData[ClassCount][class_model_path], 256, model_path);
        
        ClassData[ClassCount][class_alpha_initial] = KvGetNum(kvClassData, "alpha_initial", ZR_CLASS_DEFAULT_ALPHA_INITIAL);
        ClassData[ClassCount][class_alpha_damaged] = KvGetNum(kvClassData, "alpha_damaged", ZR_CLASS_DEFAULT_ALPHA_DAMAGED);
        ClassData[ClassCount][class_alpha_damage] = KvGetNum(kvClassData, "alpha_damage", ZR_CLASS_DEFAULT_ALPHA_DAMAGE);
        
        
        /* Hud */
        KvGetString(kvClassData, "overlay_path", overlay_path, sizeof(overlay_path), ZR_CLASS_DEFAULT_OVERLAY_PATH);
        strcopy(ClassData[ClassCount][class_overlay_path], 256, overlay_path);
        
        ClassData[ClassCount][class_nvgs] = bool:KvGetNum(kvClassData, "nvgs", ZR_CLASS_DEFAULT_NVGS);
        ClassData[ClassCount][class_fov] = KvGetNum(kvClassData, "fov", ZR_CLASS_DEFAULT_FOV);
        
        
        /* Effects */
        ClassData[ClassCount][class_napalm_time] = KvGetFloat(kvClassData, "napalm_time", ZR_CLASS_DEFAULT_NAPALM_TIME);
        
        
        /* Player behaviour */
        ClassData[ClassCount][class_immunity_mode] = KvGetNum(kvClassData, "immunity_mode", ZR_CLASS_DEFAULT_IMMUNITY_MODE);
        ClassData[ClassCount][class_immunity_amount] = KvGetFloat(kvClassData, "immunity_amount", ZR_CLASS_DEFAULT_IMMUNITY_AMOUNT);
        ClassData[ClassCount][class_no_fall_damage] = bool:KvGetNum(kvClassData, "no_fall_damage", ZR_CLASS_DEFAULT_NO_FALL_DAMAGE);
        
        ClassData[ClassCount][class_health] = KvGetNum(kvClassData, "health", ZR_CLASS_DEFAULT_HEALTH);
        ClassData[ClassCount][class_health_regen_interval] = KvGetFloat(kvClassData, "health_regen_interval", ZR_CLASS_DEFAULT_HEALTH_REGEN_INTERVAL);
        ClassData[ClassCount][class_health_regen_amount] = KvGetNum(kvClassData, "health_regen_amount", ZR_CLASS_DEFAULT_HEALTH_REGEN_AMOUNT);
        ClassData[ClassCount][class_health_infect_gain] = KvGetNum(kvClassData, "health_infect_bonus", ZR_CLASS_DEFAULT_HEALTH_INFECT_GAIN);
        ClassData[ClassCount][class_kill_bonus] = KvGetNum(kvClassData, "kill_bonus", ZR_CLASS_DEFAULT_KILL_BONUS);
        
        ClassData[ClassCount][class_speed] = KvGetFloat(kvClassData, "speed", ZR_CLASS_DEFAULT_SPEED);
        ClassData[ClassCount][class_knockback] = KvGetFloat(kvClassData, "knockback", ZR_CLASS_DEFAULT_KNOCKBACK);
        ClassData[ClassCount][class_jump_height] = KvGetFloat(kvClassData, "jump_height", ZR_CLASS_DEFAULT_JUMP_HEIGHT);
        ClassData[ClassCount][class_jump_distance] = KvGetFloat(kvClassData, "jump_distance", ZR_CLASS_DEFAULT_JUMP_DISTANCE);
        
        // Validate the class attributes.
        ClassErrorFlags = ClassValidateAttributes(ClassCount);
        if (ClassErrorFlags > 0)
        {
            // There's one or more invalid class attributes. Disable the class
            // and log an error message.
            ClassData[ClassCount][class_enabled] = false;
            if (LogFlagCheck(LOG_CORE_EVENTS, LOG_MODULE_CLASSES))
            {
                ZR_LogMessageFormatted(-1, "classes", "load", "Invalid class at index %d. Class error flags: %d.", LOG_FORMAT_TYPE_ERROR, ClassCount, ClassErrorFlags);
            }
        }
        
        // Update the counter.
        ClassCount++;
    } while (KvGotoNextKey(kvClassData));
    
    // Validate team requirements.
    if (!ClassValidateTeamRequirements())
    {
        SetFailState("The class configuration doesn't match the team requirements.");
    }
    
    // Validate team default requirements.
    if (!ClassValidateTeamDefaults())
    {
        SetFailState("Couldn't find a default class for one or more teams. At least one class per team must be marked as default.");
    }
    
    // Cache class data.
    ClassReloadDataCache();
}

/**
 * Updates the class data cache. Original values are retrieved from ClassData.
 *
 * @return      True on success, false otherwise.
 */
bool:ClassReloadDataCache()
{
    /*
     * TODO: This must be done in a safe way, because the plugin may read from
     *       the cache at any time. The plugin might read attributes at the
     *       same time when the cache is reloaded. There's a chance for
     *       corrupted attributes at that exact moment.
     */
    
    if (ClassCount == 0)
    {
        return false;
    }
    
    for (new classindex = 0; classindex < ClassCount; classindex++)
    {
        /* General */
        ClassDataCache[classindex][class_enabled] = ClassData[classindex][class_enabled];
        ClassDataCache[classindex][class_team] = ClassData[classindex][class_team];
        ClassDataCache[classindex][class_team_default] = ClassData[classindex][class_team_default];
        strcopy(ClassDataCache[classindex][class_name], 64, ClassData[classindex][class_name]);
        strcopy(ClassDataCache[classindex][class_description], 256, ClassData[classindex][class_description]);
        
        /* Model */
        strcopy(ClassDataCache[classindex][class_model_path], 256, ClassData[classindex][class_model_path]);
        ClassDataCache[classindex][class_alpha_initial] = ClassData[classindex][class_alpha_initial];
        ClassDataCache[classindex][class_alpha_damaged] = ClassData[classindex][class_alpha_damaged];
        ClassDataCache[classindex][class_alpha_damage] = ClassData[classindex][class_alpha_damage];
        
        /* Hud */
        strcopy(ClassDataCache[classindex][class_overlay_path], 256, ClassData[classindex][class_overlay_path]);
        ClassDataCache[classindex][class_nvgs] = ClassData[classindex][class_nvgs];
        ClassDataCache[classindex][class_fov] = ClassData[classindex][class_fov];
        
        /* Effects */
        ClassDataCache[classindex][class_napalm_time] = ClassData[classindex][class_napalm_time];
        
        /* Player behaviour */
        ClassDataCache[classindex][class_immunity_mode] = ClassData[classindex][class_immunity_mode];
        ClassDataCache[classindex][class_immunity_amount] = ClassData[classindex][class_immunity_amount];
        ClassDataCache[classindex][class_no_fall_damage] = ClassData[classindex][class_no_fall_damage];
        ClassDataCache[classindex][class_health] = ClassData[classindex][class_health];
        ClassDataCache[classindex][class_health_regen_interval] = ClassData[classindex][class_health_regen_interval];
        ClassDataCache[classindex][class_health_regen_amount] = ClassData[classindex][class_health_regen_amount];
        ClassDataCache[classindex][class_health_infect_gain] = ClassData[classindex][class_health_infect_gain];
        ClassDataCache[classindex][class_kill_bonus] = ClassData[classindex][class_kill_bonus];
        ClassDataCache[classindex][class_speed] = ClassData[classindex][class_speed];
        ClassDataCache[classindex][class_knockback] = ClassData[classindex][class_knockback];
        ClassDataCache[classindex][class_jump_height] = ClassData[classindex][class_jump_height];
        ClassDataCache[classindex][class_jump_distance] = ClassData[classindex][class_jump_distance];
    }
    
    return true;
}

/**
 * Clears the players class data and copies data from the specified class data
 * cache.
 *
 * @param client        The client index.
 * @param classindex    The index of the class to read from.
 * @param cachetype     Optional. Specifies what class cache to read from.
 *                      Options: ZR_CLASS_CACHE_ORIGINAL (unchanged class
 *                      data), ZR_CLASS_CACHE_MODIFIED (default, modified class
 *                      data).
 * @return      True if successful, false otherwise.
 */
bool:ClassReloadPlayerCache(client, classindex, cachetype = ZR_CLASS_CACHE_MODIFIED)
{
    // Validate indexes.
    if (!ClassValidateIndex(classindex) || !IsClientPlayer(client))
    {
        return false;
    }
    
    switch (cachetype)
    {
        case ZR_CLASS_CACHE_ORIGINAL:
        {
            /* General */
            ClassPlayerCache[client][class_enabled] = ClassData[classindex][class_enabled];
            ClassPlayerCache[client][class_team] = ClassData[classindex][class_team];
            ClassPlayerCache[client][class_team_default] = ClassData[classindex][class_team_default];
            strcopy(ClassPlayerCache[client][class_name], 64, ClassData[classindex][class_name]);
            strcopy(ClassPlayerCache[client][class_description], 256, ClassData[classindex][class_description]);
            
            /* Model */
            strcopy(ClassPlayerCache[client][class_model_path], 256, ClassData[classindex][class_model_path]);
            ClassPlayerCache[client][class_alpha_initial] = ClassData[classindex][class_alpha_initial];
            ClassPlayerCache[client][class_alpha_damaged] = ClassData[classindex][class_alpha_damaged];
            ClassPlayerCache[client][class_alpha_damage] = ClassData[classindex][class_alpha_damage];
            
            /* Hud */
            strcopy(ClassPlayerCache[client][class_overlay_path], 256, ClassData[classindex][class_overlay_path]);
            ClassPlayerCache[client][class_nvgs] = ClassData[classindex][class_nvgs];
            ClassPlayerCache[client][class_fov] = ClassData[classindex][class_fov];
            
            /* Effects */
            ClassPlayerCache[client][class_napalm_time] = ClassData[classindex][class_napalm_time];
            
            /* Player behaviour */
            ClassPlayerCache[client][class_immunity_mode] = ClassData[classindex][class_immunity_mode];
            ClassPlayerCache[client][class_immunity_amount] = ClassData[classindex][class_immunity_amount];
            ClassPlayerCache[client][class_no_fall_damage] = ClassData[classindex][class_no_fall_damage];
            ClassPlayerCache[client][class_health] = ClassData[classindex][class_health];
            ClassPlayerCache[client][class_health_regen_interval] = ClassData[classindex][class_health_regen_interval];
            ClassPlayerCache[client][class_health_regen_amount] = ClassData[classindex][class_health_regen_amount];
            ClassPlayerCache[client][class_health_infect_gain] = ClassData[classindex][class_health_infect_gain];
            ClassPlayerCache[client][class_kill_bonus] = ClassData[classindex][class_kill_bonus];
            ClassPlayerCache[client][class_speed] = ClassData[classindex][class_speed];
            ClassPlayerCache[client][class_knockback] = ClassData[classindex][class_knockback];
            ClassPlayerCache[client][class_jump_height] = ClassData[classindex][class_jump_height];
            ClassPlayerCache[client][class_jump_distance] = ClassData[classindex][class_jump_distance];
        }
        case ZR_CLASS_CACHE_MODIFIED:
        {
            /* General */
            ClassPlayerCache[client][class_enabled] = ClassDataCache[classindex][class_enabled];
            ClassPlayerCache[client][class_team] = ClassDataCache[classindex][class_team];
            ClassPlayerCache[client][class_team_default] = ClassDataCache[classindex][class_team_default];
            strcopy(ClassPlayerCache[client][class_name], 64, ClassDataCache[classindex][class_name]);
            strcopy(ClassPlayerCache[client][class_description], 256, ClassDataCache[classindex][class_description]);
            
            /* Model */
            strcopy(ClassPlayerCache[client][class_model_path], 256, ClassDataCache[classindex][class_model_path]);
            ClassPlayerCache[client][class_alpha_initial] = ClassDataCache[classindex][class_alpha_initial];
            ClassPlayerCache[client][class_alpha_damaged] = ClassDataCache[classindex][class_alpha_damaged];
            ClassPlayerCache[client][class_alpha_damage] = ClassDataCache[classindex][class_alpha_damage];
            
            /* Hud */
            strcopy(ClassPlayerCache[client][class_overlay_path], 256, ClassDataCache[classindex][class_overlay_path]);
            ClassPlayerCache[client][class_nvgs] = ClassDataCache[classindex][class_nvgs];
            ClassPlayerCache[client][class_fov] = ClassDataCache[classindex][class_fov];
            
            /* Effects */
            ClassPlayerCache[client][class_napalm_time] = ClassDataCache[classindex][class_napalm_time];
            
            /* Player behaviour */
            ClassPlayerCache[client][class_immunity_mode] = ClassDataCache[classindex][class_immunity_mode];
            ClassPlayerCache[client][class_immunity_amount] = ClassDataCache[classindex][class_immunity_amount];
            ClassPlayerCache[client][class_no_fall_damage] = ClassDataCache[classindex][class_no_fall_damage];
            ClassPlayerCache[client][class_health] = ClassDataCache[classindex][class_health];
            ClassPlayerCache[client][class_health_regen_interval] = ClassDataCache[classindex][class_health_regen_interval];
            ClassPlayerCache[client][class_health_regen_amount] = ClassDataCache[classindex][class_health_regen_amount];
            ClassPlayerCache[client][class_health_infect_gain] = ClassDataCache[classindex][class_health_infect_gain];
            ClassPlayerCache[client][class_kill_bonus] = ClassDataCache[classindex][class_kill_bonus];
            ClassPlayerCache[client][class_speed] = ClassDataCache[classindex][class_speed];
            ClassPlayerCache[client][class_knockback] = ClassDataCache[classindex][class_knockback];
            ClassPlayerCache[client][class_jump_height] = ClassDataCache[classindex][class_jump_height];
            ClassPlayerCache[client][class_jump_distance] = ClassDataCache[classindex][class_jump_distance];
        }
        default:
        {
            // Invalid cache specified.
            return false;
        }
    }
    
    return true;
}

/**
 * Sets default class indexes for each team on all players, or a single player
 * if specified.
 *
 * @param client    Optional. The client index.
 */
ClassClientSetDefaultIndexes(client = -1)
{
    new classindex = GetDefaultClassIndex();    // Old class system. Not removed for backwards compatibility.
    
    // Get indexes.
    new zombieindex = ClassGetDefaultSpawnClass(ZR_CLASS_TEAM_ZOMBIES);
    new humanindex = ClassGetDefaultSpawnClass(ZR_CLASS_TEAM_HUMANS);
    new adminindex = ClassGetDefaultSpawnClass(ZR_CLASS_TEAM_ADMINS);
    
    // Validate zombie class index.
    if (!ClassValidateIndex(zombieindex))
    {
        // Invalid class index. Fall back to default class in class config and
        // log a warning.
        if (LogFlagCheck(LOG_CORE_EVENTS, LOG_MODULE_CLASSES))
        {
            ZR_LogMessageFormatted(-1, "Classes", "SetDefaultIndexes", "Warning: Failed to get default zombie class, falling back to default class. Check spelling in \"zr_classes_default_zombie\".", LOG_FORMAT_TYPE_ERROR);
        }
        
        // Use default class.
        zombieindex = ClassGetDefaultClass(ZR_CLASS_TEAM_ZOMBIES);
    }
    
    // Validate human class index.
    if (!ClassValidateIndex(humanindex))
    {
        // Invalid class index. Fall back to default class in class config and
        // log a warning.
        if (LogFlagCheck(LOG_CORE_EVENTS, LOG_MODULE_CLASSES))
        {
            ZR_LogMessageFormatted(-1, "Classes", "SetDefaultIndexes", "Warning: Failed to get default human class, falling back to default class. Check spelling in \"zr_classes_default_human\".", LOG_FORMAT_TYPE_ERROR);
        }
        
        // Use default class.
        humanindex = ClassGetDefaultClass(ZR_CLASS_TEAM_HUMANS);
    }
    
    // Validate admin class index.
    if (!ClassValidateIndex(adminindex))
    {
        // Invalid class index. Fall back to default class in class config and
        // log a warning.
        if (LogFlagCheck(LOG_CORE_EVENTS, LOG_MODULE_CLASSES))
        {
            ZR_LogMessageFormatted(-1, "Classes", "SetDefaultIndexes", "Warning: Failed to get default admin class, falling back to default class. Check spelling in \"zr_classes_default_admin\".", LOG_FORMAT_TYPE_ERROR);
        }
        
        // Use default class.
        adminindex = ClassGetDefaultClass(ZR_CLASS_TEAM_ADMINS);
    }
    
    // Check if a client is specified.
    if (client > 0)
    {
        // Set the old array for backwards compatibility while introducing the
        // new class system.
        pClass[client] = classindex;
        
        ClassSelected[client][ZR_CLASS_TEAM_ZOMBIES] = zombieindex;
        ClassSelected[client][ZR_CLASS_TEAM_HUMANS] = humanindex;
        ClassSelected[client][ZR_CLASS_TEAM_ADMINS] = adminindex;
        
        // Copy human class data to player cache.
        ClassReloadPlayerCache(client, humanindex);
    }
    else
    {
        // No client specified. Loop through all players.
        for (new clientindex = 1; clientindex <= MAXPLAYERS; clientindex++)
        {
            // Set the old array for backwards compatibility while introducing the
            // new class system.
            pClass[clientindex] = classindex;
            
            ClassSelected[clientindex][ZR_CLASS_TEAM_ZOMBIES] = zombieindex;
            ClassSelected[clientindex][ZR_CLASS_TEAM_HUMANS] = humanindex;
            ClassSelected[clientindex][ZR_CLASS_TEAM_ADMINS] = adminindex;
            
            // Copy human class data to player cache.
            ClassReloadPlayerCache(client, humanindex);
        }
    }
}