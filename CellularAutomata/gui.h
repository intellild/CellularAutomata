#pragma once

#include<boost/multi_array.hpp>
#include"GacLib/GacUIWindows.h"

class CAWindow : 
	public GuiWindow
{
protected:
	bool on_auto_update = false;
	GuiButton *StepButton, *AutoButton;
	GuiComboBoxListControl *comboViewBurn,*comboViewLive;
	ComPtr<ID2D1SolidColorBrush> LiveBrush, DeadBrush,BlackBrush;

protected:
	void StepButton_OnClick(GuiGraphicsComposition* sender, GuiEventArgs& arguments);
	void AutoButton_OnClick(GuiGraphicsComposition* sender, GuiEventArgs& arguments);

	void comboViewBurn_SelectedIndexChanged(GuiGraphicsComposition* sender, GuiEventArgs& arguments);
	void comboViewLive_SelectedIndexChanged(GuiGraphicsComposition* sender, GuiEventArgs& arguments);

	// arguments.rt is ID2D1RenderTarget.
	void element_Rendering(GuiGraphicsComposition* sender, GuiDirect2DElementEventArgs& arguments);

	// The render target is going to be destroyed, any binded resources should be released.
	void element_BeforeRenderTargetChanged(GuiGraphicsComposition* sender, GuiDirect2DElementEventArgs& arguments);

	// The new render target is prepared, any binded resources are allowed to recreate now.
	void element_AfterRenderTargetChanged(GuiGraphicsComposition* sender, GuiDirect2DElementEventArgs& arguments);

public:
	CAWindow();
};

