#pragma once

#include <frodo.h>



class RTest : public FD::Application, public FD::EventListener {
private:
	void OnCreateWindow() override;
	void OnInit() override;
	void OnRender() override;
	void OnUpdate(float delta) override;
	void OnTick() override;

	bool OnKeyboardActionKeyPressed(FD::FD_KEY key) override;
private:
	FD::SimpleRenderer* renderer;
	FD::UserCamera* camera;
	FD::Entity3D* cube;
	FD::Entity3D* floor;
	FD::Entity3D* cube2;
	FD::DirectionalLight* dLight;
	FD::PointLight* pLight;

	uint32 fps;
public:
	RTest() : Application() {}
};