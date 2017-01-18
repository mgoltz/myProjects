import java.util.Scanner;

// minutes to years and days digits

public class chap2_PE7 {
	public static void main(String[] args) {

		final int HoursPerDay = 24;
		final int MinutesPerHour = 60;
		final int DaysPerYear = 365;

		// read in int from console
		Scanner input = new Scanner(System.in);
		System.out.print("Enter the number of minutes: ");
		int minutes = input.nextInt();

		int years = minutes / (MinutesPerHour * HoursPerDay * DaysPerYear);
		int leftOverMinutes = minutes % (MinutesPerHour * HoursPerDay * DaysPerYear);
		int days = leftOverMinutes / (MinutesPerHour * HoursPerDay);

		// print out results
		System.out.println(minutes + " minutes in approximately " + years + " years and " + days + " days.");
	}
}