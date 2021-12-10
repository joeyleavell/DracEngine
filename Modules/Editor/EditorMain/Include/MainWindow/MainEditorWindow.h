#include "EditorWindow.h"

namespace Ry
{


    class MainEditorWindow : public EditorWindow
    {
    public:

        MainEditorWindow(){};

        int32 GetViewportWidth();
		int32 GetViewportHeight();

        void Init() override;
        void Update(float Delta) override;
        void Render() override;

    };


}