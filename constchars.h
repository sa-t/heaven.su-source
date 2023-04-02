#pragma once

const char* AimType[] =
{
	"Hitbox",
	"Nearest hitbox"
};

const char* LegitHitbox[] =
{
	"Head",
	"Neck",
	"Pelvis",
	"Stomach",
	"Lower chest",
	"Chest",
	"Upper chest"
};

const char* LegitSelection[] =
{
	"Near crosshair",
	"Lowest health",
	"Highest damage",
	"Lowest distance"
};

const char* baim_cond[] =
{
	"Standing",
	"Slow walking",
	"Lethal",
	"In air"
};

const char* legs[] =
{
	"Avoid slide",
	"Slide",
	"Break"
};

const char* movement_type[] =
{
	"Stand",
	"Slow walk",
	"Move",
	"Air"
};

const char* RCSType[] =
{
	"Always on",
	"On target"
};

const char* selection[] =
{
	"Cycle",
	"Near crosshair",
	"Lowest distance",
	"Lowest health",
	"Highest damage"
};

const char* bodyaimlevel[] =
{
	"Low",
	"Medium",
	"High"
};

const char* bodyaim[] =
{
	"In air",
	"On high velocity",
	"On lethal",
	"On double tap",
	"If unresolved",
	"Prefer"
};

const char* autostop_modifiers[] =
{
	"Between shots",
	"Force accuracy"
};

const char* hitboxes[] =
{
	"Head",
	"Chest",
	"Stomach",
	"Pelvis",
	"Arms",
	"Legs",
	"Feet"
};

const char* pitch[] =
{
	"None",
	"Minimum",
	"Maximum",
	"Zero",
};

const char* yaw[] =
{
	"Static",
	"Jitter",
	"Spin"
};

const char* desync[] =
{
	"None",
	"Static",
	"Jitter"
};

const char* lby_type[] =
{
	"Static",
	"Opposite"
};

const char* baseangle[] =
{
	"Local view",
	"At targets"
};

const char* proj_combo[] =
{
	"Icon",
	"Text",
	"Box",
	"Glow"
};

const char* weaponplayer[] =
{
	"Icon",
	"Text"
};

const char* weaponesp[] =
{
	"Icon",
	"Text",
	"Box",
	"Distance",
	"Glow",
	"Ammo"
};

const char* hitmarkers[] =
{
	"Crosshair",
	"World"
};

const char* glowtarget[] =
{
	"Enemy",
	"Teammate",
	"Local"
};

const char* glowtype[] =
{
	"Standard",
	"Pulse",
	"Inner"
};

const char* local_chams_type[] =
{
	"Real",
	"Desync",
	"Arms",
	"Weapon"
};

const char* chamsvisact[] =
{
	"Visible",
	"Invisible"
};

const char* chamsvis[] =
{
	"Visible"
};

const char* chamstype[] =
{
	"Normal",
	"Flat",
	"Glow"
};

const char* flags[] =
{
	"Money",
	"Armor",
	"Defuse kit",
	"Scoped",
	"Fakeducking",
	"Bomb carrier"
};

const char* team_type[] =
{
	"Enemy",
	"Team",
	"Local"
};

const char* removals[] =
{
	"Scope",
	"Zoom",
	"Smoke",
	"Flash",
	"Recoil",
	"Landing bob",
	"Postprocessing",
	"Fog"
};

const char* skybox[] =
{
	"None",
	"Tibet",
	"Baggage",
	"Italy",
	"Aztec",
	"Vertigo",
	"Daylight",
	"Daylight 2",
	"Clouds",
	"Clouds 2",
	"Gray",
	"Clear",
	"Canals",
	"Cobblestone",
	"Assault",
	"Clouds dark",
	"Night",
	"Night 2",
	"Night flat",
	"Dusty",
	"Rainy",
	"Custom"
};

const char* mainwep[] =
{
	"None",
	"Auto",
	"AWP",
	"SSG 08"
};

const char* secwep[] =
{
	"None",
	"Dual Berettas",
	"Deagle/Revolver",
	"Tec-9/Five-SeveN"
};

const char* strafes[] =
{
	"None",
	"Legit",
	"Directional",
	"Rage"
};

const char* events_output[] =
{
	"Console",
	"Chat"
};

const char* events[] =
{
	"Player hits",
	"Items",
	"Bomb"
};

const char* grenades[] =
{
	"Grenades",
	"Armor",
	"Taser",
	"Defuser"
};

const char* fakelags[] =
{
	"Static",
	"Random",
	"Dynamic",
	"Fluctuate"
};

const char* lagstrigger[] =
{
	"Slow walk",
	"Move",
	"Air",
	"Peek"
};

const char* player_models[] =
{
	"None",
	"Cmdr. Davida 'Goggles' Fernandez | SEAL Frogman",
	"Cmdr. Frank 'Wet Sox' Baroud | SEAL Frogman",
	"Lieutenant Rex Krikey | SEAL Frogman",
	"Michael Syfers | FBI Sniper",
	"Operator | FBI SWAT",
	"Special Agent Ava | FBI",
	"Markus Delrow | FBI HRT",
	"Sous-Lieutenant Medic | Gendarmerie Nationale",
	"Chem-Haz Capitaine | Gendarmerie Nationale",
	"Chef d'Escadron Rouchard | Gendarmerie Nationale",
	"Aspirant | Gendarmerie Nationale",
	"Officer Jacques Beltram | Gendarmerie Nationale",
	"D Squadron Officer | NZSAS",
	"B Squadron Officer | SAS",
	"Seal Team 6 Soldier | NSWC SEAL",
	"Buckshot | NSWC SEAL",
	"Lt. Commander Ricksaw | NSWC SEAL",
	"Blueberries' Buckshot | NSWC SEAL",
	"3rd Commando Company | KSK",
	"Two Times' McCoy | TACP Cavalry",
	"Two Times' McCoy | USAF TACP",
	"Primeiro Tenente | Brazilian 1st Battalion",
	"Cmdr. Mae 'Dead Cold' Jamison | SWAT",
	"1st Lieutenant Farlow | SWAT",
	"John 'Van Healen' Kask | SWAT",
	"Bio-Haz Specialist | SWAT",
	"Sergeant Bombson | SWAT",
	"Chem-Haz Specialist | SWAT",
	"Lieutenant 'Tree Hugger' Farlow | SWAT",
	"Getaway Sally | The Professionals",
	"Number K | The Professionals",
	"Little Kev | The Professionals",
	"Safecracker Voltzmann | The Professionals",
	"Bloody Darryl The Strapped | The Professionals",
	"Sir Bloody Loudmouth Darryl | The Professionals",
	"Sir Bloody Darryl Royale | The Professionals",
	"Sir Bloody Skullhead Darryl | The Professionals",
	"Sir Bloody Silent Darryl | The Professionals",
	"Sir Bloody Miami Darryl | The Professionals",
	"Street Soldier | Phoenix",
	"Soldier | Phoenix",
	"Slingshot | Phoenix",
	"Enforcer | Phoenix",
	"Mr. Muhlik | Elite Crew",
	"Prof. Shahmat | Elite Crew",
	"Osiris | Elite Crew",
	"Ground Rebel | Elite Crew",
	"The Elite Mr. Muhlik | Elite Crew",
	"Trapper | Guerrilla Warfare",
	"Trapper Aggressor | Guerrilla Warfare",
	"Vypa Sista of the Revolution | Guerrilla Warfare",
	"Col. Mangos Dabisi | Guerrilla Warfare",
	"Arno The Overgrown | Guerrilla Warfare",
	"Medium Rare' Crasswater | Guerrilla Warfare",
	"Crasswater The Forgotten | Guerrilla Warfare",
	"Elite Trapper Solman | Guerrilla Warfare",
	"The Doctor' Romanov | Sabre",
	"Blackwolf | Sabre",
	"Maximus | Sabre",
	"Dragomir | Sabre",
	"Rezan The Ready | Sabre",
	"Rezan the Redshirt | Sabre",
	"Dragomir | Sabre Footsoldier"
};

const char* tracer_mode[] =
{
	"None",
	"Line",
	"Beam"
};

const char* watermark_items[] =
{
	"Cheat",
	"Framerate",
	"Ping",
	"Ticks",
	"Clock"
};