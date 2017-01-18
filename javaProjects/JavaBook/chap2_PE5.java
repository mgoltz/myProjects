import java.util.Scanner;

// feet to meters

public class chap2_PE5 {
	public static void main(String[] args) {


		// read in double from console
		Scanner input = new Scanner(System.in);
		System.out.print("Enter subtotal and gratuity rate: ");
		double subtotal = input.nextDouble();
		double gratuityRate = input.nextDouble();

		// calculate meters
		double gratuity = gratuityRate / 100 * subtotal;
		gratuity = (int)(gratuity * 100) / 100.0;
		double total = subtotal + gratuity;
		total = (int)(total * 100) /  100.0;

		// print out results
		System.out.println("The gratuity is " + gratuity + "  and total is " + total);
	}
}