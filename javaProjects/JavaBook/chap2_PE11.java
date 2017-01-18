import java.util.Scanner;

public class chap2_PE11 {
	public static void main(String[] args) {

		// read in int from console
		Scanner input = new Scanner(System.in);
		System.out.print("Enter employee's name: ");
		String name = input.nextLine();
		System.out.print("Enter number of hours worked in a week: ");
		double hoursWorked = input.nextDouble();
		System.out.print("Enter hourly pay rate: ");
		double hourlyPayRate = input.nextDouble();
		System.out.print("Enter federal tax withholding rate: ");
		double fedTaxWithHold = input.nextDouble();
		System.out.print("Enter state tax withholding rate: ");
		double stateTaxWithHold = input.nextDouble();

		double grossPay = hoursWorked * hourlyPayRate;
		grossPay = (int)(grossPay * 100) / 100.0;

		double fedWithHold = grossPay * fedTaxWithHold;
		fedWithHold = (int)(fedWithHold * 100) / 100.0;
		double stateWithHold = grossPay * stateTaxWithHold;
		stateWithHold = (int)(stateWithHold * 100) / 100.0;

		System.out.println("Employee Name: " + name);
		System.out.println("Hours Worked: " + hoursWorked);
		System.out.println("Pay Rate: $" + hourlyPayRate);
		System.out.println("Gross Pay: $" + grossPay);
		System.out.println("Deduction:");
		System.out.println("   Federal Withholding (" + (fedTaxWithHold * 100) + "%): " + fedWithHold);
		System.out.println("   State Withholding (" + (stateTaxWithHold * 100) + "%): " + stateWithHold);
		System.out.println("   Total Deduction: $" + (stateWithHold + fedWithHold) );
		System.out.println("Net Pay: $" + (grossPay - stateWithHold - fedWithHold) );
		
	}
}