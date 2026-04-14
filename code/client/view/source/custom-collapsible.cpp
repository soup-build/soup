// <copyright file="custom-collapsible.cpp" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

module;

#include <functional>
#include <memory>
#include <vector>

export module Soup.View:CustomCollapsible;

import ftxui;

using namespace ftxui;

namespace Soup::View
{
	// Create custom collapsible so we can style it
	// TODO: Make collapsible extensible with options
	export Component CustomCollapsible(ConstStringRef label, Component child, Ref<bool> show = false)
	{
		class Impl : public ComponentBase {
		public:
			Impl(ConstStringRef label, Component child, Ref<bool> show) : show_(show)
			{
				CheckboxOption option;
				option.transform = [](EntryState state)
				{
					auto prefix = text(state.state ? "  ▼ " : "  ▶ ");
					auto label = text(state.label);
					auto element = hbox({prefix, label});
					if (state.active) {
						element |= bold;
					}
					if (state.focused) {
						element |= color(Color::HotPink);
					}
					return element;	
				};
				Add(Container::Vertical({
					Checkbox(std::move(label), show_.operator->(), option),
					Maybe(std::move(child), show_.operator->()),
				}));
			}

			Ref<bool> show_;
		};

		return Make<Impl>(std::move(label), std::move(child), show);
	}
}
