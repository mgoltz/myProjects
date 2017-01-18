import java.util.Scanner;

// sum digits

public class chap2_PE6 {
	public static void main(String[] args) {


		// read in double from console
		Scanner input = new Scanner(System.in);
		System.out.print("Enter integer between 0 - 1000: ");
		int integer = input.nextInt();

		int sum = integer % 10;
		integer = integer / 10;
		sum = sum + integer % 10;
		integer = integer / 10;
		sum = sum + integer % 10;

		// print out results
		System.out.println("The sum of the digits is: " + sum);
	}
}