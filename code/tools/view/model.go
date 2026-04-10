package main

import (
	"sync"

	"charm.land/bubbles/v2/key"
	"charm.land/bubbles/v2/list"
	tea "charm.land/bubbletea/v2"
	"charm.land/lipgloss/v2"
)

type styles struct {
	app lipgloss.Style
	title lipgloss.Style
	statusMessage lipgloss.Style
}

func newStyles(darkBG bool) styles {
	lightDark := lipgloss.LightDark(darkBG)

	return styles{
		app: lipgloss.NewStyle().
			Padding(1, 2),
		title: lipgloss.NewStyle().
			Foreground(lipgloss.Color("#FFFDF5")).
			Background(lipgloss.Color("#25A065")).
			Padding(0, 1),
		statusMessage: lipgloss.NewStyle().
			Foreground(lightDark(lipgloss.Color("#04B575"), lipgloss.Color("#04B575"))),
	}
}

type item struct {
	title string
	description string
}

func (i item) Title() string { return i.title }
func (i item) Description() string { return i.description }
func (i item) FilterValue() string { return i.title }

type listKeyMap struct {
	toggleSpinner    key.Binding
	toggleHelpMenu   key.Binding
}

func newListKeyMap() *listKeyMap {
	return &listKeyMap{
		toggleSpinner: key.NewBinding(
			key.WithKeys("s"),
			key.WithHelp("s", "toggle spinner"),
		),
		toggleHelpMenu: key.NewBinding(
			key.WithKeys("H"),
			key.WithHelp("H", "toggle help"),
		),
	}
}

type model struct {
	styles styles
	darkBG bool
	width, height int
	once *sync.Once
	list list.Model
	keys *listKeyMap
	delegateKeys *delegateKeyMap
	showDetails bool
}

func (m model) Init() tea.Cmd {
	return tea.Batch(
		tea.RequestBackgroundColor,
	)
}

func (m *model) updateListProperties() {
	// Update list size.
	h, v := m.styles.app.GetFrameSize()
	m.list.SetSize(m.width-h, m.height-v)

	// Update the model and list styles.
	m.styles = newStyles(m.darkBG)
	m.list.Styles.Title = m.styles.title
}

func (m model) Update(msg tea.Msg) (tea.Model, tea.Cmd) {
	var cmds []tea.Cmd

	switch msg := msg.(type) {
	case tea.BackgroundColorMsg:
		m.darkBG = msg.IsDark()
		m.updateListProperties()
		return m, nil

	case tea.WindowSizeMsg:
		m.width, m.height = msg.Width, msg.Height
		m.updateListProperties()
		return m, nil
	}

	switch msg := msg.(type) {
	case tea.KeyPressMsg:
		// Don't match any of the keys below if we're actively filtering.
		if m.list.FilterState() == list.Filtering {
			break
		}

		switch {
		case key.Matches(msg, m.keys.toggleSpinner):
			cmd := m.list.ToggleSpinner()
			return m, cmd
		case key.Matches(msg, m.keys.toggleHelpMenu):
			m.list.SetShowHelp(!m.list.ShowHelp())
			return m, nil
		}
	}

	// This will also call our delegate's update function.
	newListModel, cmd := m.list.Update(msg)
	m.list = newListModel
	cmds = append(cmds, cmd)

	return m, tea.Batch(cmds...)
}

func (m model) View() tea.View {
	listView := m.styles.app.Render(m.list.View())

	var views []string
	views = append(views, listView)
	views = append(views, listView)
	views = append(views, listView)

	v := tea.NewView(lipgloss.JoinHorizontal(lipgloss.Top, views...))
	v.AltScreen = true
	return v
}

func initialModel() model {
    graph := loadGraph()

	// Initialize the model and list.
	m := model{}
	m.styles = newStyles(false) // default to dark background styles

	delegateKeys := newDelegateKeyMap()
	listKeys := newListKeyMap()

	// Make initial list of items.
	items := []list.Item{}
    for _, element := range graph.Packages {
        items = append(items, item{title: element.Name, description: element.PackageRoot})
	}

	// Setup list.
	delegate := newItemDelegate(delegateKeys, &m.styles)
	groceryList := list.New(items, delegate, 0, 0)
	groceryList.Title = "Build Graph"
	groceryList.Styles.Title = m.styles.title
	groceryList.AdditionalFullHelpKeys = func() []key.Binding {
		return []key.Binding{
			listKeys.toggleSpinner,
			listKeys.toggleHelpMenu,
		}
	}

	m.list = groceryList
	m.keys = listKeys
	m.delegateKeys = delegateKeys
	m.showDetails = false

	return m
}
