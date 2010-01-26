
class Object </cppClass="Object"/>
{
	m_name = "Object Test";
	function testCallback() {
		::print(this);
		::print(this.m_name);
		::print("testCallback\n");
	}
	
	static Start = {
		function main() {
			return gotoState(Idle);
		}
	}
	
	static Idle = {
		function main() {
			for (local i = 0; i < 10; i++) {
				sleep(2);
				::print("idle state");
			}
			
			return gotoState(Busy);
		}
		
		function onTouch() {
			::print(this);
			::print(this.m_name);
			::print("testCallback\n");
		}
	}
	
	static Busy = {
		function main() {
			for (local i = 0; i < 10; i++) {
				sleep(1);
				::print("busy state");
			}
			
			return gotoState(Idle);
		}
		
		function onTouch(actor) {
			switchState(Idle);
		}
	}
// 	state Idle
// 	{
// 		entry() {
// 			for (local i = 0; i < 10; i++) {
// 				Sleep(2);
// 				::print("idle state");
// 			}
// 			
// 			goto Busy;
// 		}
// 		
// 		event onTouch(actor) {
// 			switchState(Busy);
// 		}
// 	}
// 	
// 	state Busy
// 	{
// 		entry() {
// 			for (local i = 0; i < 10; i++) {
// 				Sleep(1);
// 				::print("busy state");
// 			}
// 			
// 			goto Idle;
// 		}
// 		
// 		event onTouch(actor) {
// 			switchState(Idle);
// 		}
// 	}
}

AX_REGISTER_CLASS("Object");