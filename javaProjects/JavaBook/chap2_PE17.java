import java.util.Scanner;

public class chap2_PE17 {
	public static void main(String[] args) {

		// read in  from console
		Scanner input = new Scanner(System.in);
		System.out.print("Enter the temperature in Fahrenheit: ");
		double temp = input.nextDouble();
		System.out.print("Enter the wind speed miles per hour: ");
		double windSpeed = input.nextDouble();
		
		double windChillTemp = 35.74 + 0.6215*temp - 35.75*Math.pow(windSpeed, 0.16) + 0.4275*temp*Math.pow(windSpeed, 0.16);

		System.out.println("The wind chill index is " + (int)(windChillTemp * 100)/100.0);
	}
}