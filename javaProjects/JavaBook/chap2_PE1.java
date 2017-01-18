import java.util.Scanner;

// celcius to fahrenheit converter

public class chap2_PE1 {
	public static void main(String[] args) {
		// read in double from console
		Scanner input = new Scanner(System.in);
		System.out.print("Enter an celsius temperature: ");
		// convert to 
		double cels = input.nextDouble();
		double fahren = (9.0/5) * cels + 32;
		// print out results
		System.out.println(cels + " celsius is " + fahren + " fahrenheit");
	}
}