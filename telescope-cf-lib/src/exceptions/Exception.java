
package exceptions;

public class Exception
{
	/**
	 * 
	 */
	private static final long serialVersionUID = -6697081603494519019L;
	protected Throwable throwable;

	/**
	 * Method 'BLException'
	 * 
	 * @param message
	 */
	public Exception(String message)
	{
		//super(message);
	}

	/**
	 * Method 'BLException'
	 * 
	 * @param message
	 * @param throwable
	 */
	public Exception(String message, Throwable throwable)
	{
		//super(message);
		this.throwable = throwable;
	}

	/**
	 * Method 'getCause'
	 * 
	 * @return Throwable
	 */
	public Throwable getCause()
	{
		return throwable;
	}

}
