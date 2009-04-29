/*
 * ============================================================================
 *
 *   Zombie:Reloaded
 *
 *   File:        spawnprotect.inc
 *   Description: Protects late-joining players from zombies for x seconds.
 *
 * ============================================================================
 */

/**
 * Array for storing spawn protect timer handles per client.
 */
new Handle:tSpawnProtect[MAXPLAYERS + 1];

/**
 * Array for storing time left for spawn protection per client.
 */
new pSpawnProtectTime[MAXPLAYERS + 1];
 
/**
 * Client is joining the server.
 * 
 * @param client    The client index.
 */
SpawnProtectClientInit(client)
{
    // Reset timer handle.
    tSpawnProtect[client] = INVALID_HANDLE;
}

/**
 * Client is spawning into the game.
 * 
 * @param client    The client index.
 */   
SpawnProtectOnClientSpawn(client)
{
    // Disable spawn protection on client.
    bInfectImmune[client][INFECT_TYPE_NORMAL] = false;
    
    // If timer is currently running, kill it.
    if (tSpawnProtect[client] != INVALID_HANDLE)
    {
        KillTimer(tSpawnProtect[client]);
    }
    
    // Reset timer handle.
    tSpawnProtect[client] = INVALID_HANDLE;
    
    // If client isn't on a team, then stop.
    new team = GetClientTeam(client);
    if (team != CS_TEAM_T && team != CS_TEAM_CT)
    {
        return;
    }
    
    // If zombie hasn't spawned, then stop.
    if (!g_bZombieSpawned)
    {
        return;
    }
    
    // If protect cvar is disabled, then stop.
    new bool:protect = GetConVarBool(g_hCvarsList[CVAR_SPAWNPROTECT]);
    if (!protect)
    {
        return;
    }
    
    // If player respawns as human, and either cvar zr_suicide_world_damage or the client
    // wasn't killed by world is false, then continue on to protect client.
    new bool:respawn_zombie = GetConVarBool(g_hCvarsList[CVAR_RESPAWN_ZOMBIE]);
    if (!respawn_zombie && !RespawnKilledByWorld(client))
    {
        // Set spawn protect flag on client.
        bInfectImmune[client][INFECT_TYPE_NORMAL] = true;
        
        // Set improved attributes
        // (Move to cvar?)
        ToolsSetClientAlpha(client, 0);
        ToolsSetClientLMV(client, 600.0);
        
        // Set time left to zr_protect_time's value.
        new protect_time = GetConVarInt(g_hCvarsList[CVAR_SPAWNPROTECT_TIME]);
        pSpawnProtectTime[client] = protect_time;
        
        // Tell client they are being protected.
        ZR_PrintToChat(client, "Spawn protection begin", protect_time);
        
        // Send time left in a hud message.
        ZR_HudHint(client, "Spawn Protect", pSpawnProtectTime[client]);
        
        // Start repeating timer.
        tSpawnProtect[client] = CreateTimer(1.0, SpawnProtectTimer, client, TIMER_FLAG_NO_MAPCHANGE|TIMER_REPEAT);
    }
}

/**
 * Client has been killed.
 * 
 * @param client    The client index.
 */
SpawnProtectOnClientDeath(client)
{
    // If timer is running, kill it.
    if (tSpawnProtect[client] != INVALID_HANDLE)
    {
        KillTimer(tSpawnProtect[client]);
    }
    
    // Reset timer handle.
    tSpawnProtect[client] = INVALID_HANDLE;
}

/**
 * Timer callback function, countdown for spawn protection.
 * 
 * @param timer     The timer handle.
 * @param client    The client index.
 */    
public Action:SpawnProtectTimer(Handle:timer, any:client)
{
    // If client leaves, then stop timer.
    if (!IsClientInGame(client))
    {
        return Plugin_Stop;
    }
    
    // If client has become a zombie, then stop timer.
    if (!InfectIsClientHuman(client))
    {
        return Plugin_Stop;
    }
    
    // Decrement time left.
    pSpawnProtectTime[client]--;
    
    // Print time left to client.
    ZR_HudHint(client, "Spawn Protect", pSpawnProtectTime[client]);
    
    // Time has expired.
    if (pSpawnProtectTime[client] <= 0)
    {
        // Remove protect flag.
        bInfectImmune[client][INFECT_TYPE_NORMAL] = false;
        
        // Tell client spawn protection is over.
        ZR_HudHint(client, "Spawn protection end");
        
        // Fix attributes.
        // TODO: Set class attributes.
        ToolsSetClientAlpha(client, 255);
        ToolsSetClientLMV(client, 300.0);
        
        // Clear timer handle.
        tSpawnProtect[client] = INVALID_HANDLE;
        
        // Stop timer.
        return Plugin_Stop;
    }
    
    // Allow timer to continue repeating.
    return Plugin_Continue;
}