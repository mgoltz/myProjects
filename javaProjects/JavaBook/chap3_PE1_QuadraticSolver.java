import java.util.Scanner;

public class chap3_PE1_QuadraticSolver {
	public static void main(String[] args) {
		// user inputs a, b, and c
		Scanner input = new Scanner(System.in);
		System.out.print("Enter value for a: ");
		double a = input.nextDouble();
		System.out.print("Enter value for b: ");
		double b = input.nextDouble();
		System.out.print("Enter value for c: ");
		double c = input.nextDouble();

		// calculate, evaluate discriminant
		double discrim = b*b - 4*a*c;

		if (discrim > 0.0) {
			double r1 = (-b + Math.pow(discrim, 0.5)) / (2*a);
			double r2 = (-b - Math.pow(discrim, 0.5)) / (2*a);
			System.out.printf("The roots are %4.5f and %4.5f\n", r1, r2);
		} else if (discrim == 0.0) {
			double r1 = (-b + Math.pow(discrim, 0.5)) / (2*a);
			System.out.printf("The root is %4.5f\n", r1);
		} else System.out.println("The equation has no real roots");
	}
}