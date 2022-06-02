#pragma once

#include "Resource.h"

namespace Hachiko
{
    class ResourceStateMachine : public Resource
    {
    public:
        struct Clip
        {
            std::string name;
            UID animation = 0;
            bool loop = false;

            Clip() {};
            Clip(const std::string& name, UID animation, bool loop) : name(name), animation(animation), loop(loop) {};
        };

        void AddClip(const std::string& name, UID animation, bool loop);
        int FindClip(const std::string& name) const;
        void RemoveClip(const std::string& name);
        void EditClipAnimation(const std::string& name, UID newAnimation);
        void EditClipLoop(const std::string& name, bool newLoop);
        void SetClipName(unsigned int index, const std::string& name)
        {
            clips[index].name = name;
        }

        const std::string& GetClipName(unsigned int index) const
        {
            return clips[index].name;
        }

        UID GetClipRes(unsigned int index) const
        {
            return clips[index].animation;

        }
        unsigned int GetNumClips() const
        {
            return unsigned int(clips.size());
        }
        const std::string& GetNodeClip(unsigned int index) const
        {
            return nodes[index].clip;
        }

        struct Node
        {
            std::string name;
            std::string clip;

            int nodeIndex;
            int inputIndex;

            Node() {};
            Node(const std::string& name, const std::string& clip) : name(name), clip(clip) {};
        };

        void AddNode(const std::string& name, const std::string& clip);
        int FindNode(const std::string& name) const;
        std::vector<int> FindNodeTransitions(const std::string& name) const;
        void RemoveNode(const std::string& name);
        void EditNodeClip(const std::string& name, const std::string& newClip);
        void SetNodeName(unsigned int index, const std::string& name)
        {
            nodes[index].name = name;
        }
        void SetNodeClip(unsigned int index, const std::string& clip)
        {
            nodes[index].clip = clip;
        }

        unsigned int GetDefaultNode() const
        {
            return default_node;
        }

        unsigned int GetNumNodes() const
        {
            return unsigned int(nodes.size());
        }

        void SetDefaultNode(unsigned int d)
        {
            default_node = d;
        }

        const std::string& GetNodeName(unsigned int index) const
        {
            return nodes[index].name;
        }

        struct Transition
        {
            std::string source;
            std::string target;
            std::string trigger;
            unsigned int interpolationTime = 0;

            Transition() {};
            Transition(const std::string& source, const std::string& target,const std::string& trigger, unsigned int interpolationTime) : source(source), target(target), trigger(trigger), interpolationTime(interpolationTime) {};
        };

        void AddTransition(const std::string& source, const std::string& target, const std::string& trigger, unsigned int interpolationTime);
        int FindTransitionWithTrigger(const std::string& source, const std::string& trigger) const;
        int FindTransitionWithTarget(const std::string& source, const std::string& target) const;
        std::vector<struct Transition> FindTransitionsFromNode(const std::string& name) const;
        void RemoveTransitionWithTrigger(const std::string& source, const std::string& trigger);
        void RemoveTransitionWithTarget(const std::string& source, const std::string& target);
        void EditTransitionSource(const std::string& newSource, const std::string& target, const std::string& trigger);
        void EditTransitionTarget(const std::string& source, const std::string& newTarget, const std::string& trigger);
        void EditTransitionTrigger(const std::string& source, const std::string& target, const std::string& newTrigger);
        void EditTransitionInterpolationTime(const std::string& source, const std::string& target, unsigned int newInterpolationTime);
        unsigned int GetNumTransitions() const
        {
            return unsigned int(transitions.size());
        }
        const std::string& GetTransitionSource(unsigned int index) const
        {
            return transitions[index].source;
        }
        const std::string& GetTransitionTarget(unsigned int index) const
        {
            return transitions[index].target;
        }
        const std::string& GetTransitionTrigger(unsigned int index) const
        {
            return transitions[index].trigger;
        }
        unsigned int GetTransitionBlend(unsigned int index) const
        {
            return transitions[index].interpolationTime;
        }
        void SetTransitionTrigger(unsigned int index, const std::string& trigger)
        {
            transitions[index].trigger = trigger;
        }
        void SetTransitionBlend(unsigned int index, unsigned int blend)
        {
            transitions[index].interpolationTime = blend;
        }
        void RemoveTransition(unsigned int index);

    public:
        ResourceStateMachine(UID uid);
        ~ResourceStateMachine() override;

        std::string state_m_name;

        std::vector<Clip> clips;
        std::vector<Node> nodes;
        std::vector<Transition> transitions;

        unsigned int default_node = 0;
    };
} // namespace Hachiko
