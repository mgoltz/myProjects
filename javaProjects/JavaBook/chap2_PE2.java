import java.util.Scanner;

// area and volume of cylinder

public class chap2_PE2 {
	public static void main(String[] args) {

		final double PI = 3.14159; // 'final' = java keyword to declare a constant

		// read in double from console
		Scanner input = new Scanner(System.in);
		System.out.print("Enter radius of cylinder: ");
		double radius = input.nextDouble();
		System.out.print("Enter length of cylinder: ");
		double length = input.nextDouble();

		// calculate area and volume
		double area = radius * radius * PI;
		double volume = area * length;
		// print out results
		System.out.println("The area is: " + area + "\nThe volume is: " + volume);
	}
}