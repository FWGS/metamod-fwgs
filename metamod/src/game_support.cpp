#include "precompiled.h"
#include "build.h"
#include "library_suffix.h"
#include <array>
#include <algorithm>
#include <filesystem>

// Adapted from adminmod h_export.cpp:
//! this structure contains a list of supported mods and their dlls names
//! To add support for another mod add an entry here, and add all the
//! exported entities to link_func.cpp
const game_modinfo_t g_known_games[] = {
	// name/gamedir linux_so        win_dll         desc
	//
	// Previously enumerated in this sourcefile, the list is now kept in a
	// separate file, generated based on game information stored in a
	// convenient db.
	{ "action",             "dlls/ahl_i386.so",         "dlls/ahl.dll",                   "Action Half-Life"                                 }, // + director's cut [2016], updated linux binary name
	{ "ag",                 "dlls/ag_i386.so",          "dlls/ag.dll",                    "Adrenaline Gamer (Original)"                      }, // .so file by oririnal game
	{ "ag",                 "dlls/ag.so",               "dlls/ag.dll",                    "Adrenaline Gamer (OpenAG)"                        }, // .so file by OpenAG fork
	{ "asheep",             nullptr,                    "dlls/hl.dll",                    "Azure Sheep"                                      }, // have no linux binary found!
	{ "bdef",               "dlls/server.so",           "dlls/server.dll",                "Base Defense"                                     }, // placed in normal dll folder [2017]
	{ "bg",                 "dlls/bg.so",               "dlls/bg.dll",                    "The Battle Grounds"                               },
	{ "bhl",                nullptr,                    "dlls/bhl.dll",                   "Brutal Half-Life"                                 }, // have no linux binary found!
	{ "brainbread",         nullptr,                    "dlls/bb.dll",                    "Brain Bread"                                      }, // have no linux binary found!
	{ "bshift",             "dlls/bshift.so",           "dlls/hl.dll",                    "Half-Life: Blue Shift"                            }, // ok
	{ "bumpercars",         nullptr,                    "dlls/hl.dll",                    "Bumper Cars"                                      }, // have no linux binary found!
	{ "buzzybots",          nullptr,                    "dlls/bb.dll",                    "BuzzyBots"                                        }, // have no linux binary found!
	{ "ckf3",               nullptr,                    "dlls/mp.dll",                    "Chicken Fortress 3"                               }, // have no linux binary found!, checked all versions (latest - Alpha 4)
	{ "cs10",               nullptr,                    "dlls/mp.dll",                    "Counter-Strike 1.0"                               }, // have no linux binary found!
	{ "csv15",              nullptr,                    "dlls/mp.dll",                    "Counter-Strike 1.5"                               }, // have no linux binary found!
	{ "cstrike",            "dlls/cs.so",               "dlls/mp.dll",                    "Counter-Strike 1.6"                               }, // ok
	{ "czero",              "dlls/cs.so",               "dlls/mp.dll",                    "Counter-Strike:Condition Zero"                    }, // ok
	{ "czeror",             "dlls/cz.so",               "dlls/cz.dll",                    "Counter-Strike:Condition Zero Deleted Scenes"     }, // ok
	{ "dcrisis",            "dlls/dc_i386.so",          "dlls/dc.dll",                    "Desert Crisis"                                    }, // updated linux binary name [2010]
	{ "decay",              nullptr,                    "dlls/decay.dll",                 "Half-Life: Decay"                                 }, // have no linux binary!
	{ "dmc",                "dlls/dmc.so",              "dlls/dmc.dll",                   "Deathmatch Classic"                               }, // ok
	{ "dod",                "dlls/dod.so",              "dlls/dod.dll",                   "Day of Defeat"                                    }, // ok
	{ "dpb",                "dlls/pb.i386.so",          "dlls/pb.dll",                    "Digital Paintball"                                }, // ok
	{ "esf",                "dlls/hl_i386.so",          "dlls/hl.dll",                    "Earth's Special Forces"                           }, // full linux version
	{ "esf",                "linuxdll/hl_i386.so",      "dlls/hl.dll",                    "Earth's Special Forces (Legacy)"                  }, // workaround for basic-linux version
	{ "existence",          nullptr,                    "dlls/existence.dll",             "Existence"                                        }, // have no linux binary found!
	{ "firearms",           nullptr,                    "dlls/firearms.dll",              "Firearms"                                         }, // have no linux binary found!
	{ "frontline",          "dlls/front_i386.so",       "dlls/frontline.dll",             "Frontline Force"                                  }, // updated linux binary name [2012]
	{ "gangstawars",        nullptr,                    "dlls/gwars27.dll",               "Gangsta Wars"                                     }, // have no linux binary found!
	{ "gangwars",           nullptr,                    "dlls/mp.dll",                    "Gangwars"                                         }, // have no linux binary found!
	{ "gearbox",            "dlls/opfor.so",            "dlls/opfor.dll",                 "Opposing Force"                                   }, //ok
	{ "globalwarfare",      "dlls/gw_i386.so",          "dlls/mp.dll",                    "Global Warfare"                                   }, //updated linux binary name [2012]
	{ "goldeneye",          nullptr,                    "dlls/mp.dll",                    "Goldeneye"                                        }, // have no linux binary found!
	{ "hcfrenzy",           "dlls/hcfrenzy.so",         "dlls/hcfrenzy.dll",              "Headcrab Frenzy"                                  },
	{ "hl15we",             "dlls/hl.so",               "dlls/hl.dll",                    "Half-Life 1.5: Weapon Edition"                    },
	{ "hlrally",            "dlls/hlr.so",              "dlls/hlrally.dll",               "HL-Rally"                                         },
	{ "holywars",           "dlls/hl.so",               "dlls/holywars.dll",              "Holy Wars"                                        },
	{ "hostileintent",      "dlls/hl.so",               "dlls/hl.dll",                    "Hostile Intent"                                   },
	{ "ios",                "dlls/ios.so",              "dlls/ios.dll",                   "International Online Soccer"                      },
	{ "judgedm",            "dlls/judge.so",            "dlls/mp.dll",                    "Judgement"                                        },
	{ "kanonball",          "dlls/hl.so",               "dlls/kanonball.dll",             "Kanonball"                                        },
	{ "monkeystrike",       "dlls/ms.so",               "dlls/monkey.dll",                "Monkeystrike"                                     },
	{ "MorbidPR",           "dlls/morbid.so",           "dlls/morbid.dll",                "Morbid Inclination"                               },
	{ "movein",             "dlls/hl.so",               "dlls/hl.dll",                    "Move In!"                                         },
	{ "msc",                nullptr,                    "dlls/ms.dll",                    "Master Sword Continued"                           },
	{ "ns",                 "dlls/ns.so",               "dlls/ns.dll",                    "Natural Selection"                                },
	{ "nsp",                "dlls/ns.so",               "dlls/ns.dll",                    "Natural Selection Beta"                           },
	{ "og",                 "dlls/og.so",               "dlls/og.dll",                    "Over Ground"                                      },
	{ "ol",                 "dlls/ol.so",               "dlls/hl.dll",                    "Outlawsmod"                                       },
	{ "ops1942",            "dlls/spirit.so",           "dlls/spirit.dll",                "Operations 1942"                                  },
	{ "osjb",               "dlls/osjb.so",             "dlls/jail.dll",                  "Open-Source Jailbreak"                            },
	{ "outbreak",           nullptr,                    "dlls/hl.dll",                    "Out Break"                                        }, // have no linux binary found!
	{ "oz",                 "dlls/mp.so",               "dlls/mp.dll",                    "Oz Deathmatch"                                    },
	{ "paintball",          "dlls/pb.so",               "dlls/mp.dll",                    "Paintball"                                        },
	{ "penemy",             "dlls/pe.so",               "dlls/pe.dll",                    "Public Enemy"                                     },
	{ "ponreturn",          "dlls/ponr.so",             "dlls/mp.dll",                    "Point of No Return"                               },
	{ "pvk",                "dlls/hl.so",               "dlls/hl.dll",                    "Pirates, Vikings and Knights"                     },
	{ "rc2",                "dlls/rc2.so",              "dlls/rc2.dll",                   "Rocket Crowbar 2"                                 },
	{ "recbb2",             "dlls/recb.so",             "dlls/recb.dll",                  "Resident Evil : Cold Blood"                       },
	{ "rewolf",             nullptr,                    "dlls/gunman.dll",                "Gunman Chronicles"                                }, // have no linux binary found!
	{ "ricochet",           "dlls/ricochet.so",         "dlls/mp.dll",                    "Ricochet"                                         },
	{ "rockcrowbar",        "dlls/rc.so",               "dlls/rc.dll",                    "Rocket Crowbar"                                   },
	{ "rockcrowbar",        "dlls/rc_i386.so",          "dlls/rc.dll",                    "Rocket Crowbar"                                   },
	{ "rspecies",           "dlls/hl.so",               "dlls/hl.dll",                    "Rival Species"                                    },
	{ "scihunt",            "dlls/shunt.so",            "dlls/shunt.dll",                 "Scientist Hunt"                                   },
	{ "ship",               "dlls/ship.so",             "dlls/ship.dll",                  "The Ship"                                         },
	{ "si",                 "dlls/si.so",               "dlls/si.dll",                    "Science & Industry"                               },
	{ "snow",               "dlls/snow.so",             "dlls/snow.dll",                  "Snow-War"                                         },
	{ "stargatetc",         "dlls/hl.so",               "dlls/hl.dll",                    "StargateTC (Leaacy v1.x)"                         },
	{ "stargatetc",         "dlls/stc_i386.so",         "dlls/hl.dll",                    "StargateTC (v2.x)"                                },
	{ "stargatetc",         "dlls/stc_i386_opt.so",     "dlls/hl.dll",                    "StargateTC (v2.x, optimised binary)"              },
	{ "svencoop",           "dlls/hl.so",               "dlls/hl.dll",                    "Sven Coop (Legacy)"                               }, // Metamod-r have problems with mod!
	{ "svencoop",           "dlls/server.so",           "dlls/server.dll",                "Sven Coop (Steam)"                                }, // Metamod-r have problems with mod!
	{ "swarm",              "dlls/swarm.so",            "dlls/swarm.dll",                 "Swarm"                                            },
	{ "tfc",                "dlls/tfc.so",              "dlls/tfc.dll",                   "Team Fortress Classic"                            },
	{ "thewastes",          "dlls/thewastes.so",        "dlls/thewastes.dll",             "The Wastes"                                       },
	{ "timeless",           "dlls/pt.so",               "dlls/timeless.dll",              "Project Timeless"                                 },
	{ "tod",                "dlls/hl.so",               "dlls/hl.dll",                    "Tour of Duty"                                     },
	{ "trainhunters",       "dlls/th.so",               "dlls/th.dll",                    "Train Hunters"                                    },
	{ "ts",	                "dlls/ts_i686.so",          "dlls/mp.dll",                    "The Specialists"                                  },
	{ "ts",                 "dlls/ts_i386.so",          "dlls/mp.dll",                    "The Specialists"                                  },
	{ "tt",                 "dlls/tt.so",               "dlls/tt.dll",                    "The Trenches"                                     },
	{ "underworld",         "dlls/uw.so",               "dlls/uw.dll",                    "Underworld Bloodline"                             },
	{ "valve",              "dlls/hl.so",               "dlls/hl.dll",                    "Half-Life"                                        },
	{ "vs",                 "dlls/vs.so",               "dlls/mp.dll",                    "VampireSlayer"                                    },
	{ "wantedhl",           "dlls/hl.so",               "dlls/wanted.dll",                "Wanted!"                                          },
	{ "wizardwars",         "dlls/wizardwars_i486.so",  "dlls/wizardwars.dll",            "Wizard Wars (Steam)"                              },
	{ "wizardwars_beta",    "dlls/wizardwars_i486.so",  "dlls/wizardwars.dll",            "Wizard Wars Beta (Steam)"                         },
	{ "wizwars",            "dlls/mp.so",               "dlls/hl.dll",                    "Wizard Wars (Legacy)"                             },
	{ "wormshl",            "dlls/wormshl_i586.so",     "dlls/wormshl.dll",               "WormsHL (Legacy)"                                 },
	{ "wormshl",            "dlls/wormshl_i686.so",     "dlls/wormshl.dll",               "WormsHL (Steam)"                                  },
	{ "zp",                 "dlls/hl_i386.so",          "dlls/mp.dll",                    "Zombie Panic"                                     },

	// End of list terminator:
	{ nullptr, nullptr, nullptr, nullptr }
};

// Find a modinfo corresponding to the given game name.
static const game_modinfo_t *lookup_game(const char *name, char *libpath, size_t bufsize)
{
	std::array<char, MAX_PATH> library_name = {};
	std::array<char, MAX_PATH> library_raw_name = {};

	for (auto& known : g_known_games) {
		if (known.name && !Q_stricmp(known.name, name)) {
#ifdef _WIN32
			const char *library_path_string = known.win_dll;
#else
			const char *library_path_string = known.linux_so;
#endif
			if (!library_path_string)
				continue;

			std::filesystem::path library_src_path(library_path_string);

#if XASH_ARCHITECTURE == ARCHITECTURE_X86 && (XASH_WIN32 || XASH_LINUX || XASH_APPLE)
			if (is_file_exists_in_gamedir(library_src_path.string().c_str()))
			{
				Q_strncpy(libpath, library_src_path.string().c_str(), bufsize);
				return &known;
			}
#else
			std::string library_stem = library_src_path.stem().string();
			std::copy_n(library_stem.c_str(), library_stem.size() + 1, library_raw_name.begin());
			COM_StripIntelSuffix(library_raw_name.data());
			COM_GenerateCommonLibraryName(library_raw_name.data(), library_name.data(), library_name.size());

			std::filesystem::path library_full_path = library_src_path.parent_path() / library_name.data();
			if (is_file_exists_in_gamedir(library_full_path.string().c_str()))
			{
				Q_strncpy(libpath, library_full_path.string().c_str(), bufsize);
				return &known;
			}
#endif
		}
	}

	// no match found
	return nullptr;
}

bool lookup_game_postfixes(gamedll_t *gamedll)
{
	char pathname[MAX_PATH];
	static char postfix_path[MAX_PATH] = "";

	Q_strlcpy(pathname, gamedll->pathname);

	// find extensions and skip
	char *pos = Q_strrchr(pathname, '.');
	if (pos) {
		*pos = '\0';
	}

	for (size_t i = 0; i < arraysize(g_platform_postfixes); i++)
	{
		postfix_path[0] = '\0';
		Q_strlcat(postfix_path, pathname);
		Q_strlcat(postfix_path, g_platform_postfixes[i]);

		if (is_file_exists_in_gamedir(postfix_path)) {
			Q_strlcpy(gamedll->pathname, postfix_path);
			Q_strlcpy(gamedll->real_pathname, postfix_path);
			gamedll->file = postfix_path;

			return true;
		}
	}

	return false;
}

// Installs gamedll from Steam cache
bool install_gamedll(char *from, const char *to)
{
	if (!from)
		return false;

	if (!to)
		to = from;

	int length_in;
	byte *cachefile = LOAD_FILE_FOR_ME(from, &length_in);

	// If the file seems to exist in the cache.
	if (cachefile) {
		int fd = open(to, O_WRONLY | O_CREAT | O_EXCL | O_BINARY, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
		if (fd < 0) {
			META_DEBUG(3, "Installing gamedll from cache: Failed to create file %s: %s", to, strerror(errno));
			FREE_FILE(cachefile);
			return false;
		}

		int length_out = write(fd, cachefile, length_in);
		FREE_FILE(cachefile);
		close(fd);

		// Writing the file was not successfull
		if (length_out != length_in) {
			META_DEBUG(3, "Installing gamedll from chache: Failed to write all %d bytes to file, only %d written: %s", length_in, length_out, strerror(errno));

			// Let's not leave a mess but clean up nicely.
			if (length_out >= 0)
				_unlink(to);

			return false;
		}

		META_LOG("Installed gamedll %s from cache.", to);
	}
	else {
		META_DEBUG(3, "Failed to install gamedll from cache: file %s not found in cache.", from);
		return false;
	}

	return true;
}

// Set all the fields in the gamedll struct, - based either on an entry in
// known_games matching the current gamedir, or on one specified manually
// by the server admin.
//
// meta_errno values:
//  - ME_NOTFOUND	couldn't recognize game
bool setup_gamedll(gamedll_t *gamedll)
{
	bool override = false;
	const game_modinfo_t *known;
	const char *knownfn = nullptr;
	char libpath[MAX_PATH];

	// First, look for a known game, based on gamedir.
	if ((known = lookup_game(gamedll->name, libpath, sizeof(libpath)))) {
		knownfn = libpath;
	}

	// Use override-dll if specified.
	if (g_config->m_gamedll) {
		Q_strlcpy(gamedll->pathname, g_config->m_gamedll);

		// If the path is relative, the gamedll file will be missing and
		// it might be found in the cache file.
		if (!is_abs_path(gamedll->pathname)) {
			char szInstallPath[MAX_PATH];
			Q_snprintf(szInstallPath, sizeof(szInstallPath), "%s/%s", gamedll->gamedir, gamedll->pathname);

			// If we could successfully install the gamedll from the cache we
			// rectify the pathname to be a full pathname.
			if (install_gamedll(gamedll->pathname, szInstallPath)) {
				Q_strlcpy(gamedll->pathname, szInstallPath);
			}
		}

		override = true;
	}
	// Else use Known-list dll.
	else if (known) {
		Q_snprintf(gamedll->pathname, sizeof(gamedll->pathname), "%s/%s", gamedll->gamedir, knownfn);
	}
	else {
		// Neither override nor known-list found a gamedll.
		return false;
	}

	// get filename from pathname
	char *cp = Q_strrchr(gamedll->pathname, '/');
	if (cp)
		cp++;
	else
		cp = gamedll->pathname;

	gamedll->file = cp;

	// If found, store also the supposed "real" dll path based on the
	// gamedir, in case it differs from the "override" dll path.
	if (known && override) {
		Q_snprintf(gamedll->real_pathname, sizeof(gamedll->real_pathname), "%s/%s", gamedll->gamedir, knownfn);
	}
	else {
		Q_strlcpy(gamedll->real_pathname, gamedll->pathname);
	}

	if (override) {
		// generate a desc
		Q_snprintf(gamedll->desc, sizeof(gamedll->desc), "%s (override)", gamedll->file);

		// log result
		META_LOG("Overriding game '%s' with dllfile '%s'", gamedll->name, gamedll->file);
	}
	else if (known) {
		Q_strlcpy(gamedll->desc, known->desc);

#if !defined(_WIN32)
		if (!is_file_exists_in_gamedir(gamedll->pathname))
		{
			// trying lookup gamedll with postfixes ie _i386.so
			if (lookup_game_postfixes(gamedll)) {
				META_DEBUG(3, "dll: Trying lookup to gamedll with postfixes was a success. Game '%s'", gamedll->pathname);
			}
		}
#endif

		META_LOG("Recognized game '%s'; using dllfile '%s'", gamedll->name, gamedll->file);
	}

	return true;
}
