/* Goal: To create a function that the SRV1 console will
	call once the zigzag gui is selected. 
	This function will  
*/
import java.util.*;

public class Srv1ZigZag
{
  public static void main( String [] args ) 
  {
  		//When turning into function set variable as private		
		int seconds = 5;
		
  		System.out.println("Robot drift right, CMD:4D3C2800");
		
		try
     	{
      	 Thread.sleep( (int) ( seconds * 1000 ) );
     	}
     	catch ( InterruptedException e )
     	{
     		 e.printStackTrace( );
     	}
		
		System.out.println("Robot drift left, CMD:4D283C00");
		
		try
     	{
      	 Thread.sleep( (int) ( seconds * 1000 ) );
     	}
     	catch ( InterruptedException e )
     	{
     		 e.printStackTrace( );
     	}
		
		System.out.println("Robot Stop, CMD:4D000000");
  }
}