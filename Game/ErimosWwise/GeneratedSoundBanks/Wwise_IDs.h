/////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Audiokinetic Wwise generated include file. Do not edit.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __WWISE_IDS_H__
#define __WWISE_IDS_H__

#include <AK/SoundEngine/Common/AkTypes.h>

namespace AK
{
    namespace EVENTS
    {
        static const AkUniqueID PLAY_COMBAT = 513571230U;
        static const AkUniqueID PLAY_CRYSTAL = 2035174344U;
        static const AkUniqueID PLAY_DASH = 2211787386U;
        static const AkUniqueID PLAY_DOOROPENS = 1196441153U;
        static const AkUniqueID PLAY_ENEMYATTACK = 1824944578U;
        static const AkUniqueID PLAY_ENEMYDIE = 2402038236U;
        static const AkUniqueID PLAY_FOOTSTEP = 1602358412U;
        static const AkUniqueID PLAY_GAMEOVER = 3174629258U;
        static const AkUniqueID PLAY_LASER = 2270376495U;
        static const AkUniqueID PLAY_LASER_HIT = 4092857357U;
        static const AkUniqueID PLAY_LOADINGAMMO = 998505444U;
        static const AkUniqueID PLAY_MELEEATTACK = 2988676654U;
        static const AkUniqueID PLAY_NAVIGATION = 3849525492U;
        static const AkUniqueID PLAY_PARASITEPICKUP = 3772493585U;
        static const AkUniqueID PLAY_PEBBLE = 1157125118U;
        static const AkUniqueID PLAY_RANGEDATTACK = 3766948527U;
        static const AkUniqueID PLAY_RECEIVEDAMAGE = 3438014598U;
        static const AkUniqueID PLAY_SHOOTNOAMMO = 1388583332U;
        static const AkUniqueID PLAY_SPLASH = 3948925255U;
        static const AkUniqueID PLAY_UICLICK = 1615720664U;
        static const AkUniqueID PLAY_UIHOVER = 2502251926U;
        static const AkUniqueID PLAY_WIND = 1020223172U;
        static const AkUniqueID STOP_COMBAT = 913896232U;
        static const AkUniqueID STOP_LASER = 738066393U;
        static const AkUniqueID STOP_PEBBLE = 707649828U;
        static const AkUniqueID STOP_WIND = 3173136834U;
    } // namespace EVENTS

    namespace STATES
    {
        namespace MUSIC_STATES
        {
            static const AkUniqueID GROUP = 1690668539U;

            namespace STATE
            {
                static const AkUniqueID BOSS = 1560169506U;
                static const AkUniqueID GAMEPLAY = 89505537U;
                static const AkUniqueID NONE = 748895195U;
                static const AkUniqueID VICTORY = 2716678721U;
            } // namespace STATE
        } // namespace MUSIC_STATES

        namespace ON
        {
            static const AkUniqueID GROUP = 1651971902U;

            namespace STATE
            {
                static const AkUniqueID NONE = 748895195U;
            } // namespace STATE
        } // namespace ON

        namespace PLAYER_LIFE
        {
            static const AkUniqueID GROUP = 3762137787U;

            namespace STATE
            {
                static const AkUniqueID ALIVE = 655265632U;
                static const AkUniqueID DEFEATED = 2791675679U;
                static const AkUniqueID NONE = 748895195U;
            } // namespace STATE
        } // namespace PLAYER_LIFE

        namespace SHUTTINGDOWN
        {
            static const AkUniqueID GROUP = 1847882743U;

            namespace STATE
            {
                static const AkUniqueID NONE = 748895195U;
            } // namespace STATE
        } // namespace SHUTTINGDOWN

        namespace START
        {
            static const AkUniqueID GROUP = 1281810935U;

            namespace STATE
            {
                static const AkUniqueID NONE = 748895195U;
            } // namespace STATE
        } // namespace START

    } // namespace STATES

    namespace SWITCHES
    {
        namespace FOOTSTEPS
        {
            static const AkUniqueID GROUP = 2385628198U;

            namespace SWITCH
            {
                static const AkUniqueID GRAVEL = 2185786256U;
                static const AkUniqueID STANDARD = 3025917178U;
                static const AkUniqueID WOOD = 2058049674U;
            } // namespace SWITCH
        } // namespace FOOTSTEPS

        namespace GAMEPLAY_SWITCH
        {
            static const AkUniqueID GROUP = 2702523344U;

            namespace SWITCH
            {
                static const AkUniqueID COMBAT = 2764240573U;
                static const AkUniqueID EXPLORE = 579523862U;
            } // namespace SWITCH
        } // namespace GAMEPLAY_SWITCH

    } // namespace SWITCHES

    namespace GAME_PARAMETERS
    {
        static const AkUniqueID ENEMYAWARE = 3963624107U;
    } // namespace GAME_PARAMETERS

    namespace BANKS
    {
        static const AkUniqueID INIT = 1355168291U;
        static const AkUniqueID ATTACKS = 3768541028U;
        static const AkUniqueID BACKGROUNDMUSIC = 626769978U;
        static const AkUniqueID ENVIROMENT = 3909959462U;
        static const AkUniqueID FOOTSTEPS = 2385628198U;
        static const AkUniqueID UI = 1551306167U;
    } // namespace BANKS

    namespace BUSSES
    {
        static const AkUniqueID MASTER_AUDIO_BUS = 3803692087U;
    } // namespace BUSSES

    namespace AUDIO_DEVICES
    {
        static const AkUniqueID NO_OUTPUT = 2317455096U;
        static const AkUniqueID SYSTEM = 3859886410U;
    } // namespace AUDIO_DEVICES

}// namespace AK

#endif // __WWISE_IDS_H__
