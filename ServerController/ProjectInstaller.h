#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Configuration::Install;


namespace RAServerController {

	[RunInstaller(true)]

	/// <summary>
	/// Summary for ProjectInstaller
	/// </summary>
	public ref class ProjectInstaller : public System::Configuration::Install::Installer
	{
	public:
		ProjectInstaller(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~ProjectInstaller()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::ServiceProcess::ServiceProcessInstaller^  serviceProcessInstaller;
	protected: 
	private: System::ServiceProcess::ServiceInstaller^  serviceInstaller;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->serviceProcessInstaller = (gcnew System::ServiceProcess::ServiceProcessInstaller());
			this->serviceInstaller = (gcnew System::ServiceProcess::ServiceInstaller());
			// 
			// serviceProcessInstaller
			// 
			this->serviceProcessInstaller->Account = System::ServiceProcess::ServiceAccount::LocalSystem;
			this->serviceProcessInstaller->Password = nullptr;
			this->serviceProcessInstaller->Username = nullptr;
			// 
			// serviceInstaller
			// 
			this->serviceInstaller->ServiceName = L"RAServerController";
			// 
			// ProjectInstaller
			// 
			this->Installers->AddRange(gcnew cli::array< System::Configuration::Install::Installer^  >(2) {this->serviceProcessInstaller, 
				this->serviceInstaller});

		}
#pragma endregion
	};
}
