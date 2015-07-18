#include"gui.h"
#include"CellularAutomataImplement.h"
#include<boost/multi_array.hpp>

void CAWindow::StepButton_OnClick(GuiGraphicsComposition * sender, GuiEventArgs & arguments)
{
	CA::getCA().update();
}

void CAWindow::AutoButton_OnClick(GuiGraphicsComposition * sender, GuiEventArgs & arguments)
{
	if (!on_auto_update)
	{
		on_auto_update = true;
		CA::getCA().begin_auto_update();
		AutoButton->SetText(L"停止");
		StepButton->SetEnabled(false);
		comboViewBurn->SetEnabled(false);
		comboViewLive->SetEnabled(false);
	}
	else
	{
		on_auto_update = false;
		CA::getCA().stop_auto_update();
		AutoButton->SetText(L"自动");
		StepButton->SetEnabled(true);
		comboViewBurn->SetEnabled(true);
		comboViewLive->SetEnabled(true);
	}
}

void CAWindow::comboViewBurn_SelectedIndexChanged(GuiGraphicsComposition * sender, GuiEventArgs & arguments)
{
	CA::getCA().set_burn(static_cast<int>(comboViewBurn->GetSelectedIndex()));
}

void CAWindow::comboViewLive_SelectedIndexChanged(GuiGraphicsComposition * sender, GuiEventArgs & arguments)
{
	CA::getCA().set_live(static_cast<int>(comboViewLive->GetSelectedIndex()));
}

void CAWindow::element_Rendering(GuiGraphicsComposition * sender, GuiDirect2DElementEventArgs & arguments)
{
	float base = 20.0f;
	float delta = 10.0f;
	auto print = [&](const CA::unit d, const int i, const int j)
	{
		if (d==1)
		{
			arguments.rt->FillRectangle(
				D2D1::Rect(
					base + j * delta,
					base + i * delta,
					base + (j + 1) * delta,
					base + (i + 1) * delta),
				LiveBrush.Obj());
		}
		else
		{
			arguments.rt->FillRectangle(
				D2D1::Rect(
					base + j * delta,
					base + i * delta,
					base + (j + 1) * delta,
					base + (i + 1) * delta),
				DeadBrush.Obj());
		}
	};
	CA::getCA().for_each(print);
}

void CAWindow::element_BeforeRenderTargetChanged(GuiGraphicsComposition * sender, GuiDirect2DElementEventArgs & arguments)
{
	LiveBrush = nullptr;
	DeadBrush = nullptr;
	BlackBrush = nullptr;
}

void CAWindow::element_AfterRenderTargetChanged(GuiGraphicsComposition * sender, GuiDirect2DElementEventArgs & arguments)
{
	ID2D1SolidColorBrush* brush;
	{
		brush = nullptr;
		arguments.rt->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Yellow), D2D1::BrushProperties(), &brush);
		LiveBrush = brush;
	}
	{
		brush = nullptr;
		arguments.rt->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), D2D1::BrushProperties(), &brush);
		DeadBrush = brush;
	}
	{
		brush = nullptr;
		arguments.rt->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), D2D1::BrushProperties(), &brush);
		BlackBrush = brush;
	}
}

CAWindow::CAWindow():
	GuiWindow(GetCurrentTheme()->CreateWindowStyle())
{
	SetText(L"元胞自动机");
	SetClientSize(Size(800, 600));
	GetBoundsComposition()->SetPreferredMinSize(Size(800, 600));

	//setup Direct2D
	{
		GuiDirect2DElement* element = GuiDirect2DElement::Create();
		element->Rendering.AttachMethod(this, &CAWindow::element_Rendering);
		element->BeforeRenderTargetChanged.AttachMethod(this, &CAWindow::element_BeforeRenderTargetChanged);
		element->AfterRenderTargetChanged.AttachMethod(this, &CAWindow::element_AfterRenderTargetChanged);

		GuiBoundsComposition* composition = new GuiBoundsComposition;
		composition->SetAlignmentToParent(Margin(0, 0, 0, 0));
		composition->SetOwnedElement(element);
		this->GetContainerComposition()->AddChild(composition);
	}

	// limit the size that the window should always show the whole content without cliping it
	this->GetContainerComposition()->SetMinSizeLimitation(GuiGraphicsComposition::LimitToElementAndChildren);

	//set up a table for gui
	{
		GuiTableComposition* table = new GuiTableComposition;

		// dock the table to fill the window
		table->SetAlignmentToParent(Margin(0, 0, 0, 0));
		table->SetCellPadding(6);

		table->SetRowsAndColumns(10, 4);
		for (int i = 0;i < 10;++i)
		{
			table->SetRowOption(i, GuiCellOption::PercentageOption(0.1));
		}
		table->SetColumnOption(0, GuiCellOption::PercentageOption(0.7));
		table->SetColumnOption(1, GuiCellOption::PercentageOption(0.1));
		table->SetColumnOption(2, GuiCellOption::PercentageOption(0.15));
		table->SetColumnOption(3, GuiCellOption::PercentageOption(0.05));

		// add the table to the window;
		this->GetContainerComposition()->AddChild(table);

		FontProperties font;

		//text burn
		{
			GuiCellComposition* cell = new GuiCellComposition;
			table->AddChild(cell);
			cell->SetSite(1, 1, 1, 1);

			GuiLabel* burnLabel = g::NewLabel();
			font = burnLabel->GetFont();
			font.size = 30;
			burnLabel->SetFont(font);
			burnLabel->SetText(L"Burn");
			cell->AddChild(burnLabel->GetBoundsComposition());
		}

		//text live
		{
			GuiCellComposition* cell = new GuiCellComposition;
			table->AddChild(cell);
			cell->SetSite(2, 1, 1, 1);

			GuiLabel* burnLabel = g::NewLabel();
			font = burnLabel->GetFont();
			font.size = 30;
			burnLabel->SetFont(font);
			burnLabel->SetText(L"Live");
			cell->AddChild(burnLabel->GetBoundsComposition());
		}

		//burn list
		{
			GuiCellComposition* cell = new GuiCellComposition;
			table->AddChild(cell);
			cell->SetSite(1, 2, 1, 1);

			GuiTextList* comboSource = g::NewTextList();
			font = comboSource->GetFont();
			font.size = 20;
			comboSource->SetFont(font);
			comboSource->GetItems().Add(new list::TextItem(L"0"));
			comboSource->GetItems().Add(new list::TextItem(L"1"));
			comboSource->GetItems().Add(new list::TextItem(L"2"));
			comboSource->GetItems().Add(new list::TextItem(L"3"));
			comboSource->GetItems().Add(new list::TextItem(L"4"));
			comboSource->GetItems().Add(new list::TextItem(L"5"));
			comboSource->GetItems().Add(new list::TextItem(L"6"));
			comboSource->GetItems().Add(new list::TextItem(L"7"));
			comboSource->GetItems().Add(new list::TextItem(L"8"));
			comboSource->SetHorizontalAlwaysVisible(false);

			comboViewBurn = g::NewComboBox(comboSource);
			font = comboViewBurn->GetFont();
			font.size = 20;
			comboViewBurn->SetFont(font);
			comboViewBurn->SetSelectedIndex(static_cast<vl::vint>(CA::getCA().get_burn()));
			comboViewBurn->GetBoundsComposition()->SetAlignmentToParent(Margin(0, 0, -1, 0));
			comboViewBurn->GetBoundsComposition()->SetPreferredMinSize(Size(90, 0));
			comboViewBurn->SelectedIndexChanged.AttachMethod(this, &CAWindow::comboViewBurn_SelectedIndexChanged);
			cell->AddChild(comboViewBurn->GetBoundsComposition());
		}

		//live list
		{
			GuiCellComposition* cell = new GuiCellComposition;
			table->AddChild(cell);
			cell->SetSite(2, 2, 1, 1);

			GuiTextList* comboSource = g::NewTextList();
			font = comboSource->GetFont();
			font.size = 20;
			comboSource->SetFont(font);
			comboSource->GetItems().Add(new list::TextItem(L"0"));
			comboSource->GetItems().Add(new list::TextItem(L"1"));
			comboSource->GetItems().Add(new list::TextItem(L"2"));
			comboSource->GetItems().Add(new list::TextItem(L"3"));
			comboSource->GetItems().Add(new list::TextItem(L"4"));
			comboSource->GetItems().Add(new list::TextItem(L"5"));
			comboSource->GetItems().Add(new list::TextItem(L"6"));
			comboSource->GetItems().Add(new list::TextItem(L"7"));
			comboSource->GetItems().Add(new list::TextItem(L"8"));
			comboSource->SetHorizontalAlwaysVisible(false);

			comboViewLive = g::NewComboBox(comboSource);
			font = comboViewLive->GetFont();
			font.size = 20;
			comboViewLive->SetFont(font);
			comboViewLive->SetSelectedIndex(static_cast<vl::vint>(CA::getCA().get_live()));
			comboViewLive->GetBoundsComposition()->SetAlignmentToParent(Margin(0, 0, -1, 0));
			comboViewLive->GetBoundsComposition()->SetPreferredMinSize(Size(90, 0));
			comboViewLive->SelectedIndexChanged.AttachMethod(this, &CAWindow::comboViewLive_SelectedIndexChanged);
			cell->AddChild(comboViewLive->GetBoundsComposition());
		}

		//setup the StepButton
		{
			GuiCellComposition* cell = new GuiCellComposition;
			table->AddChild(cell);

			// this cell is the top cell
			cell->SetSite(8, 1, 1, 2);
			StepButton = g::NewButton();
			font = StepButton->GetFont();
			font.size = 20;
			StepButton->SetFont(font);
			StepButton->SetText(L"单步");
			StepButton->GetBoundsComposition()->SetMinSizeLimitation(GuiGraphicsComposition::LimitToElementAndChildren);
			StepButton->GetBoundsComposition()->SetAlignmentToParent(Margin(0, 0, 0, 0));
			StepButton->Clicked.AttachMethod(this, &CAWindow::StepButton_OnClick);
			cell->AddChild(StepButton->GetBoundsComposition());
		}

		//setup the AutoButton
		{
			GuiCellComposition* cell = new GuiCellComposition;
			table->AddChild(cell);

			// this cell is the top cell
			cell->SetSite(7, 1, 1, 2);
			AutoButton = g::NewButton();
			font = AutoButton->GetFont();
			font.size = 20;
			AutoButton->SetFont(font);
			AutoButton->SetText(L"自动");
			AutoButton->GetBoundsComposition()->SetMinSizeLimitation(GuiGraphicsComposition::LimitToElementAndChildren);
			AutoButton->GetBoundsComposition()->SetAlignmentToParent(Margin(0, 0, 0, 0));
			AutoButton->Clicked.AttachMethod(this, &CAWindow::AutoButton_OnClick);
			cell->AddChild(AutoButton->GetBoundsComposition());
		}
	}

	MoveToScreenCenter();
}