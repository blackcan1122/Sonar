#pragma once

#include <functional>
#include <memory>
#include <string>

class EventDispatcher;
class GameThreadQueue;
struct TextureResource;

class EngineRuntimeContext
{
	friend class GameInstance;
public:
	void Configure(
		EventDispatcher* InUIEventDispatcher,
		EventDispatcher* InKeyDispatcher,
		EventDispatcher* InMouseDispatcher,
		EventDispatcher* InSaveStateDispatcher,
		EventDispatcher* InAllPurposeDispatcher,
		GameThreadQueue* InMainQueue,
		const std::string* InWorkingDirectory,
		const std::string* InResourceRoot,
		std::function<int()> InScreenWidthProvider,
		std::function<int()> InScreenHeightProvider,
		std::function<TextureResource*(const std::string&)> InResourceResolver,
		std::function<std::string(const std::string&)> InResourcePathResolver,
		std::function<std::string(const std::string&)> InShaderPathResolver,
		std::function<void(const std::string&)> InGameModeChangeRequester)
	{
		m_UIEventDispatcher = InUIEventDispatcher;
		m_KeyDispatcher = InKeyDispatcher;
		m_MouseDispatcher = InMouseDispatcher;
		m_SaveStateDispatcher = InSaveStateDispatcher;
		m_AllPurposeDispatcher = InAllPurposeDispatcher;
		m_MainQueue = InMainQueue;
		m_WorkingDirectory = InWorkingDirectory;
		m_ResourceRoot = InResourceRoot;
		m_ScreenWidthProvider = std::move(InScreenWidthProvider);
		m_ScreenHeightProvider = std::move(InScreenHeightProvider);
		m_ResourceResolver = std::move(InResourceResolver);
		m_ResourcePathResolver = std::move(InResourcePathResolver);
		m_ShaderPathResolver = std::move(InShaderPathResolver);
		m_GameModeChangeRequester = std::move(InGameModeChangeRequester);
	}

	EventDispatcher& GetUIEventDispatcher() const { return *m_UIEventDispatcher; }
	EventDispatcher& GetKeyDispatcher() const { return *m_KeyDispatcher; }
	EventDispatcher& GetMouseDispatcher() const { return *m_MouseDispatcher; }
	EventDispatcher& GetSaveStateEventDispatcher() const { return *m_SaveStateDispatcher; }
	EventDispatcher& GetAllPurposeDispatcher() const { return *m_AllPurposeDispatcher; }
	GameThreadQueue& GetMainQueue() const { return *m_MainQueue; }
	const std::string& GetWorkingDirectory() const { return *m_WorkingDirectory; }
	const std::string& GetResourceRoot() const { return *m_ResourceRoot; }
	int GetScreenWidth() const { return m_ScreenWidthProvider(); }
	int GetScreenHeight() const { return m_ScreenHeightProvider(); }
	TextureResource* GetResource(const std::string& ResourceName) const { return m_ResourceResolver(ResourceName); }
	std::string ResolveResourcePath(const std::string& RelativePath) const { return m_ResourcePathResolver(RelativePath); }
	std::string ResolveShaderPath(const std::string& ShaderFileName) const { return m_ShaderPathResolver(ShaderFileName); }
	void RequestGameModeChange(const std::string& GameModeName) const { m_GameModeChangeRequester(GameModeName); }
	void  RequestShutdown() { m_ShouldShutdown.store(true); }

private:
	EventDispatcher* m_UIEventDispatcher = nullptr;
	EventDispatcher* m_KeyDispatcher = nullptr;
	EventDispatcher* m_MouseDispatcher = nullptr;
	EventDispatcher* m_SaveStateDispatcher = nullptr;
	EventDispatcher* m_AllPurposeDispatcher = nullptr;
	GameThreadQueue* m_MainQueue = nullptr;
	const std::string* m_WorkingDirectory = nullptr;
	const std::string* m_ResourceRoot = nullptr;
	std::function<int()> m_ScreenWidthProvider;
	std::function<int()> m_ScreenHeightProvider;
	std::function<TextureResource*(const std::string&)> m_ResourceResolver;
	std::function<std::string(const std::string&)> m_ResourcePathResolver;
	std::function<std::string(const std::string&)> m_ShaderPathResolver;
	std::function<void(const std::string&)> m_GameModeChangeRequester;

	std::atomic<bool> m_ShouldShutdown{ false };
};