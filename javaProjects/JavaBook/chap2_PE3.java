import java.util.Scanner;

// feet to meters

public class chap2_PE3 {
	public static void main(String[] args) {

		final double CentiPerInches = 2.54;
		final double MetersPerCenti = 0.01;
		final int InchesPerFoot = 12;

		// read in double from console
		Scanner input = new Scanner(System.in);
		System.out.print("Enter feet: ");
		double feet = input.nextDouble();

		// calculate meters
		double meters = feet * InchesPerFoot * CentiPerInches * MetersPerCenti;

		// print out results
		System.out.println(feet + " feet is " + meters + " meters");
	}
}