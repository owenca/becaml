#ifndef BEOS
	#define BEOS
#endif
#include <stdio.h>
#include <string.h>

#include "alloc.h"
#include "callback.h"
#include "memory.h"
#include "mlvalues.h"
#include "signals.h"
#include "threads.h"

#include "glue.h"
#include <Application.h>

extern "C" 
{
 	extern sem_id ocaml_sem;
	value b_application_signature(value signature);
	value b_application_messageReceived(value application, value message);
	value b_application_postMessage(value application, value command);
	value b_application_quitRequested(value application);
	value b_application_readyToRun(value application);	
 value b_app(value unit);
 
 value b_application2(value signature, value error);
 value b_application3(value archive);
 value b_application_run (value application);
 value b_application_(value application);
 value b_app_resources(value application);
	extern sem_id callback_sem;
}

class OApplication : public BApplication//, public Glue 
{
		public :
				OApplication(/*value objet,*/  char * signature);
				~OApplication();
				void AboutRequested();
				void MessageReceived(BMessage *message);
				void ReadyToRun();
				bool QuitRequested();
};

OApplication::OApplication(/*value objet,*/ char *signature) : 
	BApplication(signature)//, Glue(/*objet*/)
{
			/*CAMLparam1(objet);
			CAMLreturn0;*/
}
OApplication::~OApplication(){
	printf("destruction de OApplication(0x%lx)\n", this);fflush(stdout);
//	b_glue_remove((void *)this);
}
void OApplication::AboutRequested() {
	//CAMLparam1(interne);
	//**acquire_sem(ocaml_sem);
	CAMLparam0();
	CAMLlocal1(app);
	//**release_sem(ocaml_sem);
	//**acquire_sem(ocaml_sem);
			caml_leave_blocking_section();
				app = caml_copy_int32((int32)this);
				////**acquire_sem(callback_sem);
					callback(*caml_named_value("OApplication::AboutRequested"),app/**interne*/);
				////**release_sem(callback_sem);
			caml_enter_blocking_section();
	//**release_sem(ocaml_sem);	
	CAMLreturn0;

}
void OApplication::MessageReceived(BMessage *message) {
	//**acquire_sem(ocaml_sem);
		CAMLparam0();
		CAMLlocal3(app_caml, mess_caml,fun);
			caml_register_global_root(&app_caml);
			caml_register_global_root(&mess_caml);
			caml_register_global_root(&fun);
	//**release_sem(ocaml_sem);

	BMessenger messenger = message->ReturnAddress();

	printf("[C++] OApplication::MessageReceived message = 0x%lX, what= %c%c%c%c\n",message, message->what >>24, message->what >>16, message->what >>8, message->what);fflush(stdout);
	
		//**acquire_sem(ocaml_sem);
		caml_leave_blocking_section();
			app_caml  = caml_copy_int32((int32) this);
			
		    mess_caml = caml_copy_int32((int32) message);
			
			fun = *caml_named_value("OApplication::MessageReceived");
			
				callback2(fun, app_caml, mess_caml);
		caml_enter_blocking_section();
	//**release_sem(ocaml_sem);
	CAMLreturn0;
}

void OApplication::ReadyToRun() {
	//**acquire_sem(ocaml_sem);
	//CAMLparam1(interne);
	CAMLparam0();
	CAMLlocal1(app_caml);
	//**release_sem(ocaml_sem);
	
	//**acquire_sem(ocaml_sem);
		caml_leave_blocking_section();
			app_caml  = caml_copy_int32((int32) this);
//			//**acquire_sem(callback_sem);
				callback(*caml_named_value("OApplication::Ready_to_run"), app_caml/* *interne*/);
//			//**release_sem(callback_sem);
		caml_enter_blocking_section();
	//**release_sem(ocaml_sem);
	CAMLreturn0;
}

bool OApplication::QuitRequested(){
	//**acquire_sem(ocaml_sem);
		CAMLparam0();
		CAMLlocal2(res, app_caml);
	//**release_sem(ocaml_sem);	
	
		//**acquire_sem(ocaml_sem);
		caml_leave_blocking_section();
			caml_register_global_root(&app_caml);
			app_caml  = caml_copy_int32((int32) this);
			caml_register_global_root(&res);
//			//**acquire_sem(callback_sem);
				printf("[C++]OApplication::QuitRequested(), appel de callback\n");fflush(stdout);
				caml_c_thread_register();
				res = caml_callback(*caml_named_value("OApplication::QuitRequested"),app_caml/* *interne*/);
				printf("[C++]OApplication::QuitRequested(), retour de callback\n");fflush(stdout);
//			//**release_sem(callback_sem);
		caml_enter_blocking_section();
		//**release_sem(ocaml_sem);
	CAMLreturn(res);
}

//*********************************

value b_application_signature(value signature)
{
//	//**acquire_sem(ocaml_sem);
		CAMLparam1(signature);
		CAMLlocal1(application); 
//	//**release_sem(ocaml_sem);
	
	OApplication *app;
	char * s = String_val(signature);
//	register_global_root(&self); 
		app = new OApplication(s);
	printf("[C] creation be_application (0x%lx)\n", app);fflush(stdout);
	
//	//**acquire_sem(ocaml_sem);
//		caml_leave_blocking_section();
			application = caml_copy_int32((int32)app);
			caml_c_thread_register();
			caml_callback(*caml_named_value("OApplication::Set_be_app"), application);
//		caml_enter_blocking_section();
//	//**release_sem(ocaml_sem);
	CAMLreturn(application);
}

//**************************
value b_application_messageReceived(value application, value message) {
	//**acquire_sem(ocaml_sem);
		CAMLparam2(application, message);
	//**release_sem(ocaml_sem);
	OApplication *app;
	BMessage *mess;
	
//	caml_leave_blocking_section();
	app	 = ((OApplication *)Int32_val(application));
	mess = (BMessage *)Int32_val(message);
	
		app->BApplication::MessageReceived(mess);

	CAMLreturn(Val_unit);
}

//************
value b_application_postMessage(value application, value command) {
	CAMLparam2(application, command);
	CAMLlocal1(res_caml);	
	OApplication *app = (OApplication *)Int32_val(application);
	uint32 com 		  = Int32_val(command);
	status_t res;

	//**release_sem(ocaml_sem);
	caml_enter_blocking_section();
		res = app->BApplication::PostMessage(com);
	//**acquire_sem(ocaml_sem);
	caml_leave_blocking_section();
	res_caml = caml_copy_int32(res);
	CAMLreturn(res_caml);

}

//*************
value b_application_quitRequested(value application) {
	CAMLparam1(application);
	CAMLlocal1(res_caml);
	status_t res;
	
	OApplication *app = (OApplication *)Int32_val(application);
	printf("[C++]OApplication->BApplication::QuitRequested()");fflush(stdout);
	caml_enter_blocking_section();
	//**release_sem(ocaml_sem);
		res = app->BApplication::QuitRequested();
	//**acquire_sem(ocaml_sem);
	caml_leave_blocking_section();
	
	res_caml = Val_bool(res);
	
	CAMLreturn(res_caml);
}

//************
value b_application_readyToRun(value application) {
	CAMLparam1(application);

	OApplication *app = (OApplication *)Int32_val(application);
	caml_enter_blocking_section();
	//**release_sem(ocaml_sem);
		app->BApplication::ReadyToRun(); //appel de la m�thode originale
	//**acquire_sem(ocaml_sem);
	caml_leave_blocking_section();
	
	CAMLreturn(Val_unit);
}

//***************************
value b_app(value unit) {
	CAMLparam1(unit);
	CAMLlocal1(application);
	caml_leave_blocking_section();
		application = caml_copy_int32((value)be_app);
	caml_enter_blocking_section();
	CAMLreturn(application);
}


value b_application2(value signature, value error) 
{
    CAMLparam0();
		printf("b_application2 non implemente\n");fflush(stdout);
	CAMLreturn(Val_unit);
}

value b_application3(value archive) 
{
	CAMLparam0();
	printf("b_application3 non implemente\n");fflush(stdout);
	CAMLreturn(Val_unit);
 }


value b_application_run(value application)
{
	CAMLparam1(application);
	CAMLlocal1(res_caml);
	status_t res;
	OApplication *app;

//	//**acquire_sem(ocaml_sem);
//	caml_leave_blocking_section();	
		app = (OApplication *)Int32_val(application);
	caml_enter_blocking_section();
//	//**release_sem(ocaml_sem);
	
	res = app->Run();
	printf("[C++] b_application_run() : retour de app->Run\n");fflush(stdout);
//	//**acquire_sem(ocaml_sem);
	caml_leave_blocking_section();
		res_caml = copy_int32(res);
		
//	caml_leave_blocking_section();
//	//**release_sem(ocaml_sem);
	printf("[C++] fin de b_application_run() \n");fflush(stdout);
	
	CAMLreturn(res_caml);
}

value b_application_(value application)
{
 CAMLparam1(application);
 OApplication *oapp = (OApplication *)Int32_val(application);
 
 caml_enter_blocking_section();
	 oapp->BApplication::~BApplication();
 caml_leave_blocking_section();
 
 CAMLreturn(Val_unit);
}

value b_app_resources(value application)
{
 CAMLparam1(application);
 CAMLlocal1(resources);
 printf("b_app_resources : copy_int a revoir\n");fflush(stdout);
 caml_leave_blocking_section();
 	resources = copy_int32((value) ((OApplication *)Int32_val(application))->BApplication::AppResources());
 caml_enter_blocking_section();

 CAMLreturn(resources);
}

