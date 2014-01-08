#include "materials.h"

#include "client/snd_public.h"
#include "renderer/tr_types.h"
#include "client/FXExport.h"

#include "qcommon/qcommon.h"
#include "qcommon/GenericParser2.h"

#include <list>
#include <string>
#include <utility>
#include <vector>

using std::list;
using std::pair;
using std::string;
using std::vector;

//-----------------------------------------------------------------
//
// Sounds, copy of CMediaHandles
//
// Primitive templates might want to use a list of sounds, shaders 
//	or models to get a bit more variation in their effects.
//
//-----------------------------------------------------------------
class Sounds {
	vector<int> _mediaList;
public:
	void addHandle(int item) {
		_mediaList.push_back(item);
	}

	int	getHandle() const {
		if (_mediaList.empty()) {
			return 0;
		}
		return _mediaList[irand(0, _mediaList.size() - 1)];
	}

	void operator=(const Sounds &that ) {
		_mediaList.clear();
		std::copy(that._mediaList.begin(), that._mediaList.end(), _mediaList.begin());
	}
};

/// Material class keeping info about a specific material and it's sounds and effects
class Material {
	typedef pair<string, Sounds> SoundPair;
	typedef pair<string, qhandle_t> EffectPair;

	list<SoundPair> _sounds;
	list<EffectPair> _effects;

	Material(const Material&); // don't allow copying
public:
	Material(CGPGroup * const group);

	sfxHandle_t getSound(const char * key) const;
	qhandle_t getEffect(const char * key) const;
};

/// Create a material from the given group read from a file.
Material::Material(CGPGroup * const materialGroup) {
	// groups are footstep, land, bouncemetal0, ..., ammoTypes
	for (CGPGroup * group = materialGroup->GetSubGroups(); group != NULL; group = group->GetNext()) {
		const char * keyName = group->GetName();

		CGPValue * value = group->FindPair("sound");
		if (value) { // add the sound(s)
			Sounds sounds;

			const char * const path = value->GetTopValue();
			sfxHandle_t sound = S_RegisterSound(path);
			if (sound) {
				// Found our base sound.
				sounds.addHandle(sound);
				this->_sounds.push_back(SoundPair(keyName, sounds));
				continue;
			}

			// Try to find some variations, add 0 and 1 to the end.
			for (char soundNr = 48; soundNr < 50; ++soundNr) {
				char localPath[MAX_QPATH];
				sprintf(localPath, "%s%c\0", path, soundNr);
				sfxHandle_t sound = S_RegisterSound(localPath);
				if (sound) {
					sounds.addHandle(sound);
				}
			}

			this->_sounds.push_back(SoundPair(keyName, sounds));
		} else if (!strcmp(keyName, "ammoTypes")) {
			// parse the ammo types
			for (CGPGroup * ammoType = group->GetSubGroups(); ammoType != NULL; ammoType = ammoType->GetNext()) {
				CGPValue * value = ammoType->FindPair("effect");
				if (value) { // add the ffect
					qhandle_t effect = FX_RegisterEffect(value->GetTopValue());
					this->_effects.push_back(EffectPair(ammoType->GetName(), effect));
				}
			}
		}
	}
}

/// Get the sound for the given key.
sfxHandle_t Material::getSound(const char * key) const {
	for (list<SoundPair>::const_iterator it = this->_sounds.begin(); it != this->_sounds.end(); ++it) {
		if (!stricmp(key, it->first.c_str())) {
			return it->second.getHandle();
		}
	}
	return 0;
}

/// Get the effect for the given key.
qhandle_t Material::getEffect(const char * key) const {
	for (list<EffectPair>::const_iterator it = this->_effects.begin(); it != this->_effects.end(); ++it) {
		if (!stricmp(key, it->first.c_str())) {
			return it->second;
		}
	}
	return 0;
}

const char * materialNames[MATERIAL_LAST] = { MATERIALS };
Material * materials[MATERIAL_LAST] = { NULL };

/// Parse and initialize our materials.
void Mat_Init(void) {
	char * buffer;
	FS_ReadFile("ext_data/generic.material", (void **) &buffer);

	CGenericParser2 parser;
	if (parser.Parse(buffer)) {
		CGPGroup * baseGroup = parser.GetBaseParseGroup();
		for (int i = 0; i < MATERIAL_LAST; ++i) {
			CGPGroup * materialGroup = baseGroup->FindSubGroup(materialNames[i]);

			if (materials[i]) {
				delete materials[i];
			}
			materials[i] = new Material(materialGroup);
		}
	}

	parser.Clean();

	FS_FreeFile((void *) buffer);
}

/// Clear all materials.
void Mat_Reset(void) {
	for (int i = 0; i < MATERIAL_LAST; ++i) {
		delete materials[i];
		materials[i] = NULL;
	}
}

/// Get the sound for the given key and material.
sfxHandle_t Mat_GetSound(char * key, int material) {
	Material * const mat = materials[material];
	if (mat) {
		return mat->getSound(key);
	}
	return 0;
}

/// Not used
qhandle_t Mat_GetDecal(char * key, int material) {
	return 0;
}

/// Not used
const float Mat_GetDecalScale(char * key, int material) {
	return 0;
}

/// Get the effect for the given key and material.
qhandle_t Mat_GetEffect(char * key, int material) {
	Material * const mat = materials[material];
	if (mat) {
		return mat->getEffect(key);
	}
	return 0;
}

/// Not used
qhandle_t Mat_GetDebris(char * key, int material) {
	return 0;
}

/// Not used
const float Mat_GetDebrisScale(char * key, int material) {
	return 0;
}