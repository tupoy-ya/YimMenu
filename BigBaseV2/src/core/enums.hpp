#pragma once

namespace big
{
	enum class CustomWeapon
	{
		NONE,
		CAGE_GUN,
		DELETE_GUN,
		GRAVITY_GUN,
		STEAL_VEHICLE_GUN,
		REPAIR_GUN,
		VEHICLE_GUN
	};
	
    static char* exlosions[] =
	{
        "GRENADE",
        "GRENADELAUNCHER",
        "STICKYBOMB", 
        "MOLOTOV", 
        "ROCKET",
        "TANKSHELL", 
        "HI_OCTANE", 
        "CAR",
        "PLANE", 
        "PETROL_PUMP",
        "BIKE", 
        "DIR_STEAM", 
        "DIR_FLAME", 
        "DIR_WATER_HYDRANT", 
        "DIR_GAS_CANISTER", 
        "BOAT", 
        "SHIP_DESTROY", 
        "TRUCK", 
        "BULLET", 
        "SMOKEGRENADELAUNCHER",
        "SMOKEGRENADE", 
        "BZGAS", 
        "FLARE",
        "GAS_CANISTER", 
        "EXTINGUISHER", 
        "_0x988620B8",
        "EXP_TAG_TRAIN",
        "EXP_TAG_BARREL",
        "EXP_TAG_PROPANE", 
        "EXP_TAG_BLIMP",
        "EXP_TAG_DIR_FLAME_EXPLODE", 
        "EXP_TAG_TANKER", 
        "PLANE_ROCKET", 
        "EXP_TAG_VEHICLE_BULLET", 
        "EXP_TAG_GAS_TANK", 
        "EXP_TAG_BIRD_CRAP", 
        "EXP_TAG_RAILGUN", 
        "EXP_TAG_BLIMP2", 
        "EXP_TAG_FIREWORK", 
        "EXP_TAG_SNOWBALL", 
        "EXP_TAG_PROXMINE", 
        "EXP_TAG_VALKYRIE_CANNON", 
        "EXP_TAG_AIR_DEFENCE", 
        "EXP_TAG_PIPEBOMB", 
        "EXP_TAG_VEHICLEMINE", 
        "EXP_TAG_EXPLOSIVEAMMO", 
        "EXP_TAG_APCSHELL", 
        "EXP_TAG_BOMB_CLUSTER", 
        "EXP_TAG_BOMB_GAS", 
        "EXP_TAG_BOMB_INCENDIARY", 
        "EXP_TAG_BOMB_STANDARD", 
        "EXP_TAG_TORPEDO", 
        "EXP_TAG_TORPEDO_UNDERWATER", 
        "EXP_TAG_BOMBUSHKA_CANNON", 
        "EXP_TAG_BOMB_CLUSTER_SECONDARY", 
        "EXP_TAG_HUNTER_BARRAGE", 
        "EXP_TAG_HUNTER_CANNON", 
        "EXP_TAG_ROGUE_CANNON", 
        "EXP_TAG_MINE_UNDERWATER", 
        "EXP_TAG_ORBITAL_CANNON", 
        "EXP_TAG_BOMB_STANDARD_WIDE", 
        "EXP_TAG_EXPLOSIVEAMMO_SHOTGUN", 
        "EXP_TAG_OPPRESSOR2_CANNON", 
        "EXP_TAG_MORTAR_KINETIC", 
        "EXP_TAG_VEHICLEMINE_KINETIC", 
        "EXP_TAG_VEHICLEMINE_EMP", 
        "EXP_TAG_VEHICLEMINE_SPIKE", 
        "EXP_TAG_VEHICLEMINE_SLICK", 
        "EXP_TAG_VEHICLEMINE_TAR", 
        "EXP_TAG_SCRIPT_DRONE", 
        "EXP_TAG_RAYGUN", 
        "EXP_TAG_BURIEDMINE",
		"EXP_TAG_SCRIPT_MISSILE",
		"EXP_TAG_RCTANK_ROCKET",
		"EXP_TAG_BOMB_WATER",
		"EXP_TAG_BOMB_WATER_SECONDARY",
		"_0xF728C4A9",
		"_0xBAEC056F",
		"EXP_TAG_FLASHGRENADE",
		"EXP_TAG_STUNGRENADE",
		"_0x763D3B3B",
		"EXP_TAG_SCRIPT_MISSILE_LARGE",
		"EXP_TAG_SUBMARINE_BIG"
    };
	

    /*enum class eExplosionType : unsigned int
    {
        DONTCARE = UINT_MAX,
        GRENADE = 0,
        GRENADELAUNCHER = 1,
        STICKYBOMB = 2,
        MOLOTOV = 3,
        ROCKET = 4,
        TANKSHELL = 5,
        HI_OCTANE = 6,
        CAR = 7,
        PLANE = 8,
        PETROL_PUMP = 9,
        BIKE = 10,
        DIR_STEAM = 11,
        DIR_FLAME = 12,
        DIR_WATER_HYDRANT = 13,
        DIR_GAS_CANISTER = 14,
        BOAT = 15,
        SHIP_DESTROY = 16,
        TRUCK = 17,
        BULLET = 18,
        SMOKEGRENADELAUNCHER = 19,
        SMOKEGRENADE = 20,
        BZGAS = 21,
        FLARE = 22,
        GAS_CANISTER = 23,
        EXTINGUISHER = 24,
        _0x988620B8 = 25,
        EXP_TAG_TRAIN = 26,
        EXP_TAG_BARREL = 27,
        EXP_TAG_PROPANE = 28,
        EXP_TAG_BLIMP = 29,
        EXP_TAG_DIR_FLAME_EXPLODE = 30,
        EXP_TAG_TANKER = 31,
        PLANE_ROCKET = 32,
        EXP_TAG_VEHICLE_BULLET = 33,
        EXP_TAG_GAS_TANK = 34,
        EXP_TAG_BIRD_CRAP = 35,
        EXP_TAG_RAILGUN = 36,
        EXP_TAG_BLIMP2 = 37,
        EXP_TAG_FIREWORK = 38,
        EXP_TAG_SNOWBALL = 39,
        EXP_TAG_PROXMINE = 40,
        EXP_TAG_VALKYRIE_CANNON = 41,
        EXP_TAG_AIR_DEFENCE = 42,
        EXP_TAG_PIPEBOMB = 43,
        EXP_TAG_VEHICLEMINE = 44,
        EXP_TAG_EXPLOSIVEAMMO = 45,
        EXP_TAG_APCSHELL = 46,
        EXP_TAG_BOMB_CLUSTER = 47,
        EXP_TAG_BOMB_GAS = 48,
        EXP_TAG_BOMB_INCENDIARY = 49,
        EXP_TAG_BOMB_STANDARD = 50,
        EXP_TAG_TORPEDO = 51,
        EXP_TAG_TORPEDO_UNDERWATER = 52,
        EXP_TAG_BOMBUSHKA_CANNON = 53,
        EXP_TAG_BOMB_CLUSTER_SECONDARY = 54,
        EXP_TAG_HUNTER_BARRAGE = 55,
        EXP_TAG_HUNTER_CANNON = 56,
        EXP_TAG_ROGUE_CANNON = 57,
        EXP_TAG_MINE_UNDERWATER = 58,
        EXP_TAG_ORBITAL_CANNON = 59,
        EXP_TAG_BOMB_STANDARD_WIDE = 60,
        EXP_TAG_EXPLOSIVEAMMO_SHOTGUN = 61,
        EXP_TAG_OPPRESSOR2_CANNON = 62,
        EXP_TAG_MORTAR_KINETIC = 63,
        EXP_TAG_VEHICLEMINE_KINETIC = 64,
        EXP_TAG_VEHICLEMINE_EMP = 65,
        EXP_TAG_VEHICLEMINE_SPIKE = 66,
        EXP_TAG_VEHICLEMINE_SLICK = 67,
        EXP_TAG_VEHICLEMINE_TAR = 68,
        EXP_TAG_SCRIPT_DRONE = 69,
        EXP_TAG_RAYGUN = 70,
        EXP_TAG_BURIEDMINE = 71,
        EXP_TAG_SCRIPT_MISSILE = 72,
        EXP_TAG_RCTANK_ROCKET = 73,
        EXP_TAG_BOMB_WATER = 74,
        EXP_TAG_BOMB_WATER_SECONDARY = 75,
        _0xF728C4A9 = 76,
        _0xBAEC056F = 77,
        EXP_TAG_FLASHGRENADE = 78,
        EXP_TAG_STUNGRENADE = 79,
        _0x763D3B3B = 80,
        EXP_TAG_SCRIPT_MISSILE_LARGE = 81,
        EXP_TAG_SUBMARINE_BIG = 82,
    };*/

	enum class eRemoteEvent
	{
		Bounty = -1906146218,
		CeoBan = 1355230914,
		CeoKick = -316948135,
		CeoMoney = 1152266822,
		ClearWantedLevel = 1187364773,
		FakeDeposit = 153488394,
		ForceMission = -1147284669,
		GtaBanner = 1659915470,
		PersonalVehicleDestroyed = 299217086,
		RemoteOffradar = -397188359,
		RotateCam = -1320260596,
		SendToCutscene = 1889984715,
		SendToIsland = -1479371259,
		SoundSpam = 1537221257,
		Spectate = -148441291,
		Teleport = 1249026189,
		TransactionError = -2041535807,
		VehicleKick = -1005623606
	};

    inline const int64_t kick_hashes[]{ -1382676328, 1256866538, -1753084819, 1119864805, -1833002148, 202252150, -1503282114, 243981125, -1836118977, -169685950, -2071141142, -149227625, 1433396036, 1608876738, 458875017, 987018372, -1587276086, 1954846099, 813647057 };

	enum class eSessionType
	{
		JOIN_PUBLIC,
		NEW_PUBLIC,
		CLOSED_CREW,
		CREW,
		CLOSED_FRIENDS = 6,
		FIND_FRIEND = 9,
		SOLO,
		INVITE_ONLY,
		JOIN_CREW,
		LEAVE_ONLINE = -1
	};

	enum class SpeedoMeter
	{
		DISABLED,
		KMH,
		MPH
	};
}