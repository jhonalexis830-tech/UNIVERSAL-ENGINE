#include "ue/editor/EditorState.hpp"

namespace ue::editor {

EditorState::EditorState(scene::Scene& scene, render::IRenderer& renderer,
                         platform::IInput& input, physics::PhysicsWorld& physics,
                         media::FrameRecorder& recorder, anim::Animator& animator)
    : scene(scene)
    , renderer(renderer)
    , input(input)
    , physics(physics)
    , recorder(recorder)
    , animator(animator) {}

} // namespace ue::editor