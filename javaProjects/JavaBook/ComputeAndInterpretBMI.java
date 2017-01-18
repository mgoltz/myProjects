import java.util.Scanner;

public class ComputeAndInterpretBMI {
	public static void main(String[] args) {

		final double KilogramsPerPound = 0.45359237;
		final double MetersPerInch = 0.0254;

		Scanner input = new Scanner(System.in);
		System.out.print("Enter weight in pounds: ");
		double weight = input.nextDouble();
		System.out.print("Enter height in inches: ");
		double height = input.nextDouble();

		double weightInKilograms = weight * KilogramsPerPound;
		double heightInMeters = height * MetersPerInch;
		double bmi = weightInKilograms / (heightInMeters * heightInMeters);

		System.out.printf("Your BMI is %5.2f\n", bmi);

		if (bmi < 16) {
			System.out.println("You are seriously underweight");
		} else if (bmi < 18) {
			System.out.println("You are underweight");
		} else if (bmi < 24) {
			System.out.println("You are normal weight");
		} else if (bmi < 29) {
			System.out.println("You are overweight");
		} else if (bmi < 35) {
			System.out.println("You are seriously overweight");
		} else System.out.println("You are gravely overweight");

	}
}