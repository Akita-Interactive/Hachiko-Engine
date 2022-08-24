#ifndef GAMEPLAY_PCH_H
#define GAMEPLAY_PCH_H

#include "framework.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <MathGeoLib.h> // Order matters, this should be included before 
// Gameplay.h

#include <utils/RandomUtil.h>

#include <components/Component.h>
#include <components/ComponentText.h>
#include <components/ComponentButton.h>
#include <components/ComponentProgressBar.h>
#include <components/ComponentAnimation.h>
#include <components/ComponentAudioSource.h>
#include <components/ComponentCamera.h>
#include <components/ComponentAgent.h>
#include <components/ComponentImage.h>
#include <components/ComponentTransform.h>
#include <components/ComponentTransform2D.h>
#include <components/ComponentPointLight.h>
#include <components/ComponentObstacle.h>
#include <utils/Bool2.h>
#include <core/particles/Particle.h>
#include <core/particles/ParticleSystem.h>
#include <components/ComponentParticleSystem.h>
#include <components/ComponentBillboard.h>

#include <core/GameObject.h>
#include <Gameplay.h>
#include <Globals.h>
#endif //GAMEPLAY_PCH_H