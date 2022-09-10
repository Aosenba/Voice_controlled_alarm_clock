#pragma once
//#include<Windows.h>
#include"hmm.h"


namespace Alarm {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace std;

	/// <summary>
	/// Summary for Form1
	/// </summary>

	public ref class Form1 : public System::Windows::Forms::Form
	{
	public:
		Form1(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			read_codebook();
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~Form1()
		{
			if (components)
			{
				delete components;
			}
		}

	protected: 

	private: System::Windows::Forms::Button^  start_btn;
	private: System::Windows::Forms::Label^  label_hour;
	private: System::Windows::Forms::Label^  label_mins;


	private: System::Windows::Forms::Label^  sec_label;
	private: System::Windows::Forms::Label^  label_day;
	private: System::Windows::Forms::Label^  label_final_time;
	private: System::Windows::Forms::Timer^  timer1;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Label^  label_snooze;
	private: System::ComponentModel::IContainer^  components;



	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(Form1::typeid));
			this->start_btn = (gcnew System::Windows::Forms::Button());
			this->label_hour = (gcnew System::Windows::Forms::Label());
			this->label_mins = (gcnew System::Windows::Forms::Label());
			this->sec_label = (gcnew System::Windows::Forms::Label());
			this->label_day = (gcnew System::Windows::Forms::Label());
			this->label_final_time = (gcnew System::Windows::Forms::Label());
			this->timer1 = (gcnew System::Windows::Forms::Timer(this->components));
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label_snooze = (gcnew System::Windows::Forms::Label());
			this->SuspendLayout();
			// 
			// start_btn
			// 
			this->start_btn->AllowDrop = true;
			this->start_btn->BackColor = System::Drawing::SystemColors::Highlight;
			this->start_btn->Cursor = System::Windows::Forms::Cursors::AppStarting;
			this->start_btn->Font = (gcnew System::Drawing::Font(L"Agency FB", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->start_btn->ForeColor = System::Drawing::SystemColors::Info;
			this->start_btn->Location = System::Drawing::Point(134, 112);
			this->start_btn->Name = L"start_btn";
			this->start_btn->Size = System::Drawing::Size(100, 30);
			this->start_btn->TabIndex = 2;
			this->start_btn->Text = L"SET";
			this->start_btn->UseVisualStyleBackColor = false;
			this->start_btn->Click += gcnew System::EventHandler(this, &Form1::start_btn_Click);
			// 
			// label_hour
			// 
			this->label_hour->AutoSize = true;
			this->label_hour->BackColor = System::Drawing::Color::Transparent;
			this->label_hour->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->label_hour->Cursor = System::Windows::Forms::Cursors::Hand;
			this->label_hour->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->label_hour->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 21.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label_hour->ForeColor = System::Drawing::SystemColors::HighlightText;
			this->label_hour->Location = System::Drawing::Point(113, 58);
			this->label_hour->Name = L"label_hour";
			this->label_hour->Padding = System::Windows::Forms::Padding(1);
			this->label_hour->Size = System::Drawing::Size(51, 37);
			this->label_hour->TabIndex = 3;
			this->label_hour->Text = L"00";
			this->label_hour->Click += gcnew System::EventHandler(this, &Form1::label1_Click);
			// 
			// label_mins
			// 
			this->label_mins->AutoSize = true;
			this->label_mins->BackColor = System::Drawing::Color::Transparent;
			this->label_mins->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->label_mins->Cursor = System::Windows::Forms::Cursors::Hand;
			this->label_mins->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 21.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label_mins->ForeColor = System::Drawing::SystemColors::HighlightText;
			this->label_mins->Location = System::Drawing::Point(166, 58);
			this->label_mins->Name = L"label_mins";
			this->label_mins->Padding = System::Windows::Forms::Padding(1);
			this->label_mins->Size = System::Drawing::Size(51, 37);
			this->label_mins->TabIndex = 4;
			this->label_mins->Text = L"00";
			this->label_mins->Click += gcnew System::EventHandler(this, &Form1::label2_Click);
			// 
			// sec_label
			// 
			this->sec_label->AutoSize = true;
			this->sec_label->BackColor = System::Drawing::Color::Transparent;
			this->sec_label->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->sec_label->Cursor = System::Windows::Forms::Cursors::Hand;
			this->sec_label->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 21.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->sec_label->ForeColor = System::Drawing::SystemColors::HighlightText;
			this->sec_label->Location = System::Drawing::Point(219, 58);
			this->sec_label->Name = L"sec_label";
			this->sec_label->Padding = System::Windows::Forms::Padding(1);
			this->sec_label->Size = System::Drawing::Size(51, 37);
			this->sec_label->TabIndex = 5;
			this->sec_label->Text = L"00";
			this->sec_label->Click += gcnew System::EventHandler(this, &Form1::label3_Click);
			// 
			// label_day
			// 
			this->label_day->AutoSize = true;
			this->label_day->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->label_day->Cursor = System::Windows::Forms::Cursors::Hand;
			this->label_day->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 15.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label_day->Location = System::Drawing::Point(272, 65);
			this->label_day->Name = L"label_day";
			this->label_day->Size = System::Drawing::Size(52, 27);
			this->label_day->TabIndex = 6;
			this->label_day->Text = L"A.M";
			this->label_day->Click += gcnew System::EventHandler(this, &Form1::label_day_Click);
			// 
			// label_final_time
			// 
			this->label_final_time->AutoSize = true;
			this->label_final_time->BackColor = System::Drawing::SystemColors::ActiveCaptionText;
			this->label_final_time->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->label_final_time->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 21.75F, System::Drawing::FontStyle::Regular, 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->label_final_time->ForeColor = System::Drawing::SystemColors::ButtonHighlight;
			this->label_final_time->Location = System::Drawing::Point(227, 165);
			this->label_final_time->Name = L"label_final_time";
			this->label_final_time->Size = System::Drawing::Size(129, 35);
			this->label_final_time->TabIndex = 7;
			this->label_final_time->Text = L"00:00:00";
			this->label_final_time->Click += gcnew System::EventHandler(this, &Form1::label_final_time_Click);
			// 
			// timer1
			// 
			this->timer1->Interval = 1000;
			this->timer1->Tick += gcnew System::EventHandler(this, &Form1::timer1_Tick);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->BackColor = System::Drawing::Color::Transparent;
			this->label1->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->label1->Font = (gcnew System::Drawing::Font(L"Agency FB", 24, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label1->ForeColor = System::Drawing::Color::White;
			this->label1->Location = System::Drawing::Point(61, 160);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(160, 41);
			this->label1->TabIndex = 8;
			this->label1->Text = L"Current Time:";
			this->label1->Click += gcnew System::EventHandler(this, &Form1::label1_Click_1);
			// 
			// label_snooze
			// 
			this->label_snooze->AutoSize = true;
			this->label_snooze->BackColor = System::Drawing::Color::Transparent;
			this->label_snooze->Font = (gcnew System::Drawing::Font(L"Agency FB", 14.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label_snooze->ForeColor = System::Drawing::SystemColors::ButtonHighlight;
			this->label_snooze->Location = System::Drawing::Point(130, 22);
			this->label_snooze->Name = L"label_snooze";
			this->label_snooze->Size = System::Drawing::Size(0, 24);
			this->label_snooze->TabIndex = 9;
			this->label_snooze->Click += gcnew System::EventHandler(this, &Form1::label_snooze_Click);
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackgroundImage = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"$this.BackgroundImage")));
			this->ClientSize = System::Drawing::Size(413, 303);
			this->Controls->Add(this->label_snooze);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->label_final_time);
			this->Controls->Add(this->label_day);
			this->Controls->Add(this->sec_label);
			this->Controls->Add(this->label_mins);
			this->Controls->Add(this->label_hour);
			this->Controls->Add(this->start_btn);
			this->Name = L"Form1";
			this->Text = L"Alarm Clock";
			this->Load += gcnew System::EventHandler(this, &Form1::Form1_Load);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
int hour,min,sec,flag;
private: System::Void Form1_Load(System::Object^  sender, System::EventArgs^  e) {
				this->timer1->Enabled = true;
				flag=0;
				sec=0;
				hour=11;
				min=10;
				
		 }
private: System::Void label2_Click(System::Object^  sender, System::EventArgs^  e) {
		
			        char str[10];
					min=alarm();
					if(min<10)
						sprintf(str,"0%d",min);
					else
						 sprintf(str,"%d",min);
				   String^ s= gcnew String(str);
				  this->label_mins->Text = s;
		 }
private: System::Void label1_Click(System::Object^  sender, System::EventArgs^  e) {
			     
                   char str[10];
				   hour=alarm();
				   if(hour<10)
				     sprintf(str,"0%d",hour);
				   else
					 sprintf(str,"%d",hour);
				   String^ s= gcnew String(str);
				  this->label_hour->Text = s;
		 }
private: System::Void label3_Click(System::Object^  sender, System::EventArgs^  e) {
		 }

private: System::Void label_day_Click(System::Object^  sender, System::EventArgs^  e) {	
			 //
			 if(strcmp(testing(2), "am")==0)
			 {
				  this->label_day->Text="A.M";
			 }
			 else
			 {
				   this->label_day->Text="P.M";
				   hour=hour+12;
			 }
				 
		 }
 private: System::Void start_btn_Click(System::Object^  sender, System::EventArgs^  e) {
			    
				  char str[10];
				 if(flag==0)
				 {
					 this->start_btn->Text = "ALARM SET"  ;	
				 }
				 else
				 {
					 if(strcmp(testing(1), "stop")==0)
					 {
						 flag=0;
						 this->start_btn->Text = "SET" ;
					 }
						 
					 else
					 {
						 this->label_snooze->Text = "snoozed for 5 mins";
						 min=min+5;
						 flag=0;
						 if(min<10)
							sprintf(str,"0%d",min);
						else
							sprintf(str,"%d",min);
					    String^ s= gcnew String(str);
					     this->label_mins->Text = s;
					 }
					     
				 }
					
			 }
private: System::Void timer1_Tick(System::Object^  sender, System::EventArgs^  e) {
			    char time_c[100];
			    int c_hour,c_min,c_sec;
			    time_t t2 = time(0);   
				tm* now = localtime(&t2);
				c_hour =now->tm_hour;
				c_min=now->tm_min;
				c_sec=now->tm_sec;
				sprintf(time_c,"%d : %d : %d ",c_hour,c_min,c_sec);
				 String^ s= gcnew String(time_c);
			    this->label_final_time->Text=s;
				if(c_hour==hour && c_min==min && c_sec == sec)
				{
					 flag=1;
					this->start_btn->Text = "STOP/SNOOZE"; 
				
				}
				if(flag==1)
				{
				  Beep(523,1000);  
				}
							
		 }
private: System::Void label1_Click_1(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void label_final_time_Click(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void label_snooze_Click(System::Object^  sender, System::EventArgs^  e) {
		 }
};
}

