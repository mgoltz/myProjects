import java.util.Scanner;

// minutes to years and days digits

public class chap2_PE8 {
	public static void main(String[] args) {

		// read in int from console
		Scanner input = new Scanner(System.in);
		System.out.print("Enter an ASCII code: ");
		int code = input.nextInt();

		// print out results
		System.out.println("The character for ASCII code " + code + " is " + (char)code);
	}
}