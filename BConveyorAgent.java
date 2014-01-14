
package engine.agent.boston;

import shared.interfaces.Component;
import shared.interfaces.boston.*;
import engine.agent.*;
import shared.*;
import transducer.*;
import transducer.Transducer;

import java.util.*;

public class BConveyorAgent extends Agent implements Conveyor{

	String name;
	Integer capacity, index;
	Transducer transducer;
	enum GlassState {OnConveyor, End, Checking, Ready, MovingOn};
	enum ConveyorStatus {On, Off}
	ConveyorStatus status;
	enum CompState {Idle, NeedRecipe, HasPart, Busy, GivingPart};
	MyComponent left, right;

	public class MyComponent {
		Component component;
		CompState state;

		public MyComponent (Component c, CompState s)
		{
			component = c;
			state = s;
		}

		public Component getComponent() {
			return component;
		}

		public CompState getState() {
			return state;
		}
	}

	public class MyGlass {
		public GlassState gState;
		public Glass glass;

		public MyGlass(GlassState l, Glass g) {
			gState = l;
			glass = g;
		}
	}

	List<MyGlass> glassList = new ArrayList<MyGlass>();

	public BConveyorAgent(String s, Integer i) {
		name = s;
		index = i;
		status = ConveyorStatus.On;
	}

	//******************MESSAGES***********************


	public void msgHereIsGlass(Component c, Glass g) {				// first receive glass
		if (left.component == c) {
			glassList.add(new MyGlass(GlassState.OnConveyor, g));
			stateChanged();
		}
	}


	public void msgGlassCanMoveOn() {							// there is a space open to move the glass
		if (!glassList.isEmpty()) {
			glassList.get(0).gState = GlassState.Ready;
			stateChanged();
		}
	}


	public void msgAreYouReady(Component src) {				//check if next sensor is ready
		if (left.component == src) {
			print("front sensor asking if ready");
			left.state = CompState.HasPart;
			stateChanged();
		}

	}

	public void msgReady() {							//move first glass to next conveyor
		if (!glassList.isEmpty()) {
			if (glassList.get(0).gState == GlassState.Checking) {
				print("got ready");
				glassList.get(0).gState = GlassState.MovingOn;
				stateChanged();
			}
		}

	}



	//*********************SCHEDULER***************

	public boolean pickAndExecuteAnAction() {



		if (!glassList.isEmpty()) {						//if glass has moved on remove from list
			if (glassList.get(0).gState == GlassState.MovingOn) {
				removeGlass(glassList.get(0));
				return true;
			}

			if (glassList.get(0).gState == GlassState.Ready) {				//check if glass can move on
				checkRight(glassList.get(0));
				return true;
			}

		}	
		if(status == ConveyorStatus.On){					//check if space is available to receive glass
			if (left.state == CompState.HasPart) {
				checkReady();
				return true;
			}
		}


		return false;
	}

	//*****************ACTIONS**********************

	private void checkReady() {
		left.state = CompState.GivingPart;
		left.component.msgReady();
	}

	private void removeGlass(MyGlass mg) {
		print("give right glass");
		right.component.msgHereIsGlass(this, mg.glass);
		glassList.remove(0);
		startConveyor();
	}



	private void checkRight(MyGlass mg) {
		mg.gState = GlassState.Checking;
		stopConveyor();
		print("asking end sensor if ready");
		right.component.msgAreYouReady(this);
	}

	private void stopConveyor(){
		Object[] args = new Object[1];
		args[0] = index;
		transducer.fireEvent(TChannel.CONVEYOR, TEvent.CONVEYOR_DO_STOP, args);
		status = ConveyorStatus.Off;
	}

	private void startConveyor(){
		Object[] args = new Object[1];
		args[0] = index;
		transducer.fireEvent(TChannel.CONVEYOR, TEvent.CONVEYOR_DO_START, args);
		status = ConveyorStatus.On;
	}


	public void addGlass(Glass g) {
		glassList.add(new MyGlass(GlassState.OnConveyor, g));	//Unit Testing only
	}

	public List<MyGlass> getGlassList() {
		return glassList;	//Unit testing only
	}

	public boolean conveyorIsOn() {
		if (status == ConveyorStatus.On) 
			return true;
		return false;
	}

	public String getName() {
		return name;
	}

	public void setTransducer(Transducer t) {
		transducer = t;
		transducer.register(this, TChannel.CONVEYOR);
	}

	public void setLeft(Component c) {
		left = new MyComponent(c, CompState.Idle);
	}

	public void setRight(Component c) {
		right = new MyComponent(c, CompState.Idle);
	}



	@Override
	public void eventFired(TChannel channel, TEvent event, Object[] args) {
		if(channel == TChannel.CONVEYOR){
			if(((Integer)(args[0])).equals(index)){
				if(event == TEvent.SENSOR_GUI_PRESSED){
					msgGlassCanMoveOn();
				}	
			}
		}
	}


}

