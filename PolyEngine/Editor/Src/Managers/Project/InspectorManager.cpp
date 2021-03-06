#include "PolyEditorPCH.hpp"

#include "Managers/CommandsImpl.hpp"

//		general
//------------------------------------------------------------------------------
InspectorManager::InspectorManager(EditorApp* app)
	: ConfigRef(Config)
{
	Config = &app->ProjectMgr->GetProjectConfig();;

	connect(app->ProjectMgr, &ProjectManager::EngineInitialized, this, &InspectorManager::EngineInitializedSlot);
	connect(app->ProjectMgr, &ProjectManager::EngineDeinitialized, this, &InspectorManager::EngineDeinitializedSlot);
	connect(app->ProjectMgr, &ProjectManager::EngineStateChanged, this, &InspectorManager::StateChangedSlot);
}

//------------------------------------------------------------------------------
void InspectorManager::InitUi()
{
	// world inspector
	WorldInspector = new WorldInspectorWidget(gApp->Ui.MainWindow, this);
	PolyDockWindow* window = new PolyDockWindow("World Inspector", WorldInspector);
	gApp->Ui.MainWindow->AddDockWindow(Qt::DockWidgetArea::LeftDockWidgetArea, window, true);

	// resource inspector
	ResourceInspector = new ResourceInspectorWidget(gApp->Ui.MainWindow, this);
	window = new PolyDockWindow("Resource Inspector", ResourceInspector);
	gApp->Ui.MainWindow->AddDockWindow(Qt::DockWidgetArea::BottomDockWidgetArea, window, true);
	window->hide();

	// viewport inspector
	ViewportInspector = new ViewportInspectorWidget(gApp->Ui.MainWindow, this);
	window = new PolyDockWindow("Viewport Inspector", ViewportInspector);
	gApp->Ui.MainWindow->AddDockWindow(Qt::DockWidgetArea::RightDockWidgetArea, window, true);

	// entity inspector
	EntityInspector = new EntityInspectorWidget(gApp->Ui.MainWindow, this);
	window = new PolyDockWindow("Entity Inspector", EntityInspector);
	gApp->Ui.MainWindow->AddDockWindow(Qt::DockWidgetArea::RightDockWidgetArea, window, true);


	WorldInspector->InitializeConnections();
	ResourceInspector->InitializeConnections();
	ViewportInspector->InitializeConnections();
	EntityInspector->InitializeConnections();
}

std::unique_ptr<IRenderingDevice> InspectorManager::GetRenderingDevice() { return ViewportInspector->GetRenderingDevice(); }
PolyEditor::IEditor* InspectorManager::GetEditor() { return ViewportInspector; }


//		engine slots
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void InspectorManager::EngineInitializedSlot(Engine* engine)
{
	EngineObj = engine;
	emit EngineInitialized();
	SceneChangedSlot(EngineObj->GetActiveScene());
}

//------------------------------------------------------------------------------
void InspectorManager::EngineDeinitializedSlot()
{
	EngineObj = nullptr;

	WorldInspector->Reset();
	ResourceInspector->Reset();
	ViewportInspector->Reset();
	EntityInspector->Reset();
	
	emit EngineDeinitialized();
}

//------------------------------------------------------------------------------
void InspectorManager::StateChangedSlot(PolyEditor::eEngineState state)
{
	EngineState = state;

	emit StateChanged();
}


//		world slots
//------------------------------------------------------------------------------
void InspectorManager::SceneChangedSlot(Scene* scene)
{
	SceneObj = scene;

	emit EntitiesSelectionChangedSlot({});
	emit WorldChanged();
}



//		entities slots
//------------------------------------------------------------------------------
void InspectorManager::EntitiesSpawnedSlot()
{
	emit EntitiesSpawned();
}

//------------------------------------------------------------------------------
void InspectorManager::EntitiesDestroyedSlot()
{
	EntitiesDestroyed();

	for (Entity* ent : SelectedEntities)
		DeferredTaskSystem::DestroyEntityImmediate(SceneObj, ent);

	EntitiesSelectionChangedSlot({});
}

//------------------------------------------------------------------------------
void InspectorManager::EntitiesModifiedSlot(Command* cmd)
{
	gApp->CommandMgr->AddCommand(cmd);

	emit EntitiesModified();
}

//------------------------------------------------------------------------------
void InspectorManager::EntitiesReparentedSlot()
{
	emit EntitiesReparented();
}

//------------------------------------------------------------------------------
void InspectorManager::EntitiesSelectionChangedSlot(Dynarray<Entity*> entities)
{
	EntitiesSelectionChangedCommand* c = new EntitiesSelectionChangedCommand();
	c->OldEntities = SelectedEntities;
	c->NewEntities = entities;
	c->Manager = this;
	gApp->CommandMgr->AddCommand(c);

	SelectedEntities = entities;
	EntitiesSelectionChanged();
}



//		components slots
//------------------------------------------------------------------------------
void InspectorManager::ComponentsAddedSlot(Dynarray<ComponentBase*> components)
{
	emit ComponentsAdded();
}

//------------------------------------------------------------------------------
void InspectorManager::ComponentsRemovedSlot(Dynarray<ComponentBase*> components)
{
	emit ComponentsRemoved();
}
