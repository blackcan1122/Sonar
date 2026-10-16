#pragma once
#include "Base/Object.hpp"
#include "Base/EventDispatcher.hpp"
#include "Base/Gridcell.hpp"
#include "Base/SoftObject.hpp"
#include <any>
#include <functional>
#include <string>
#include <vector>

class Display;

struct LayoutSpawnRequest
{
	std::string SpawnId;
	int IntegerPayload = 0;
};

struct LayoutSpawnOption
{
	std::string SpawnId;
	std::string DisplayName;
	int IntegerPayload = 0;

	LayoutSpawnRequest ToRequest() const
	{
		return { SpawnId, IntegerPayload };
	}
};

class LayoutManager : public Object
{
	AUTOBODY(LayoutManager, Object)

public:
	using DeleteDisplayCallback = std::function<void(SoftObjectPath<Display>)>;
	using CreateDisplayCallback = std::function<void(const GridCell&, const LayoutSpawnRequest&)>;

	virtual bool RegisterDisplay(SoftObjectPath<IObject> inDisplay, std::any LayoutData = {}) = 0;
	virtual bool UnregisterDisplay(SoftObjectPath<IObject> inDisplay) = 0;

	virtual void UpdateLayout() = 0;

	virtual void OnWindowResize(int newWidth, int newHeight) = 0;

	virtual void SetEnabled(bool enabled) { m_IsEnabled = enabled; }
	virtual bool IsEnabled() const { return m_IsEnabled; }

	virtual void SetShowControls(bool show) { m_ShowControls = show; }
	virtual bool IsShowingControls() const { return m_ShowControls; }

	virtual void SetDeleteDisplayCallback(DeleteDisplayCallback callback) { m_OnDeleteDisplay = std::move(callback); }
	virtual void SetCreateDisplayCallback(CreateDisplayCallback callback) { m_OnCreateDisplay = std::move(callback); }

	virtual void SetSpawnOptions(std::vector<LayoutSpawnOption> spawnOptions) { m_SpawnOptions = std::move(spawnOptions); }
	virtual const std::vector<LayoutSpawnOption>& GetSpawnOptions() const { return m_SpawnOptions; }

protected:
	void RequestDeleteDisplay(SoftObjectPath<Display> display)
	{
		if (m_OnDeleteDisplay)
		{
			m_OnDeleteDisplay(display);
		}
	}

	void RequestCreateDisplay(const GridCell& cell, const LayoutSpawnRequest& spawnRequest)
	{
		if (m_OnCreateDisplay)
		{
			m_OnCreateDisplay(cell, spawnRequest);
		}
	}

	bool m_IsEnabled = true;
	bool m_ShowControls = true;
	std::vector<LayoutSpawnOption> m_SpawnOptions;
	DeleteDisplayCallback m_OnDeleteDisplay;
	CreateDisplayCallback m_OnCreateDisplay;
};