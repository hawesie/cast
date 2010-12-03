package xsrad.logging;

public class Translate {

	/*
	public static final int _ADD = 0;
    public static final int _OVERWRITE = 1;
    public static final int _DELETE = 2;
    public static final int _GET = 3;
    public static final int _WILDCARD = 4;
	 */
	public static String translateCastActionType2XsradActionType(int type)
	{
		String ret = "NONE";
		switch(type) {
			case 0:
				ret = "add";
				break;
			case 1:
				ret = "replace";
				break;
			case 2:
				ret = "delete";
				break;
			case 3:
				ret = "get";
				break;
			case 4:
				ret = "all";
				break;
		}
		return ret;
	}
}
