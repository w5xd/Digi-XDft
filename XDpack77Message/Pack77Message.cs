using System;
using System.Linq;

namespace XDpack77
{
    namespace Pack77Message
    {
        /* This code does NOT reproduce packing and unpacking in wsjt's
         * pack77 and unpack77. Instead, it presents the results of those pack/unpack operations
         * via C# interfaces
         */
        public class Message
        {
            public const int NO_DB = -999;
            public int i3 { get; protected set; } = -1;
            public int n3 { get; protected set; } = -1;
            private ReceivedMessage asReceived; // can be null. not all messages are received
            public ReceivedMessage AsReceived { get { return asReceived; } set { asReceived = value; } }
         }

        public interface ToFromCall
        {
            String FromCall { get;  }
            bool FromCallIsHashed { get; }
            String ToCall { get;  } // can be CQ, including directed CQ
            bool ToCallIsHashed { get; }
        }

        public interface Exchange
        {  
            String Exchange { get;  }   // returns everything
            String RST { get; } // if it has a received RST, returns it. otherwise null
            int SignaldB { get; } // if it has a received signal dB return it. otherwise NO_DB
            String GridSquare { get; } // if it has a received grid square return it. otherwise null
        }

        public interface Roger
        { bool Roger { get;  } } // true return is ack FromCall to ToCall 

        public interface QSL
        {   // implicit QSL to prior message 
            String CallQSLed { get;  } // can be null (which means none) or "ALL" or ...
                                        //...an individual call independent of ToCall and FromCall
            String QslText { get; } // RRR, RR73, 73, or null
            // "R" is not a QSL
        }

        public interface IsCQ
        {
            bool SolicitsAnswers { get; } // return true if a CQ or otherwise solicits calls
        }

        // A ReceivedMessage always has a Message in its Pack77Message property
        public class ReceivedMessage
        {
            private string timeTag;
            private int dB;
            private float delay=0;
            private string hzTag;
            private int cycleNumber;
            private DateTime whenLogged;
            private string rawString;
            private Message message;
            private char separator;

            ReceivedMessage()
            {
                dB = Message.NO_DB;
                cycleNumber = -1;
            }
            public Message Pack77Message { get { return message; } } // we always have one.
            public override String ToString() 
            {
                // recreate the string used to parse.
                return String.Format("{0,6}{1,4} {2,4:0.0}{3,5} {5}  {4}", timeTag, dB, delay, hzTag, rawString, separator);
            }
            public bool Match(ReceivedMessage other)
            {
                return (rawString.Trim() == other.rawString.Trim()) &&
                        (timeTag == other.timeTag);
            }
            public String Content { get { return rawString; } }
            public String TimeTag { get { return timeTag; } }
            public int SignalDB { get { return dB; } }
            public DateTime WhenLogged { get { return whenLogged; } }
            public String RST {
                get {   // convert the dB printout to 529 through 599
                    int decib = dB;
                    int s = (decib + 36) / 6;
                    if (s < 2) s = 2;
                    if (s > 9) s = 9;
                    return String.Format("5{0}9", s);
                }
            }
            public uint Hz { get {  return UInt32.Parse(hzTag); } }
            public int CycleNumber { get { return cycleNumber; } }

            public static ReceivedMessage CreateFromReceived(int i3, int n3, string asTagged, 
                string Asreceived, int cycleNumber, char separator='~', float delayOffset = 0)
            {
                Message msg = CreateFromPacked(i3, n3, Asreceived);
                if (msg != null)
                {
                    ReceivedMessage rm = new ReceivedMessage();
                    String [] words = asTagged.Split((char[])null, StringSplitOptions.RemoveEmptyEntries);
                    if (words.Length != 4)
                        throw new Exception("CreateFromPacked parse error 1");
                    rm.timeTag = words[0];
                    rm.separator = separator;
                    Int32.TryParse(words[1], out rm.dB);
                    float.TryParse(words[2], out rm.delay);
                    rm.delay += delayOffset;
                    rm.hzTag = words[3];
                    msg.AsReceived = rm;
                    rm.rawString = Asreceived;
                    rm.message = msg;
                    rm.whenLogged = DateTime.Now;
                    rm.cycleNumber = cycleNumber;
                    return rm;
                }
                return null;
            }

            public static Message CreateFromPacked(int i3, int n3, string Asreceived)
            {
                if ((i3 == 0) && (n3 == 0))
                { return new FreeText(Asreceived, i3, n3); }

                // trim space characters
                String[] split = Asreceived.Split((char[])null, StringSplitOptions.RemoveEmptyEntries);
                if ((i3 == 0) && (n3 == 1))
                { return new Dxpedition(split, i3, n3); }
                else if ((i3 == 0) && ((n3 == 3) || (n3 == 4)))
                { return new ArrlFieldDayMessage(split, i3, n3); }
                else if ((i3 == 1) || (i3 == 2))
                { return new StandardMessage(split, i3, n3); }
                else if (i3 == 3)
                { return new RttyRoundUpMessage(split, i3, n3); }
                else if (i3 == 4)
                { return new NonStandardCallMessage(split, i3, n3); }
                return null;
            }
        }

        public class FreeText : Message
        {
            public FreeText(String freeText, int i3, int n3)
            {
                contents = freeText;
                if (contents.Length > 13)
                    contents = contents.Substring(0, 13);
                base.i3 = i3;
                base.n3 = n3;
            }
            private String contents;
            String Contents {
                get { return contents; }
                set { contents = value; }
            }
        }

        public class Dxpedition : Message, ToFromCall, Exchange, QSL
        {
            private string callAcked;
            private bool callAckedIsHashed = false;
            private string toCall;
            private bool toCallIsHashed = false;
            private string fromCall;
            private string report;
            public Dxpedition(String[] incoming, int i3, int n3)
            {
                base.i3 = i3;
                base.n3 = n3;
                // two calls, an implied RR73 to the first, a hash of my own call, and a signal strength
                // K1ABC RR73; W9XYZ <KH1/KH7Z> -12
                int which = 0;
                if (incoming.Length <= which)
                    throw new Exception("Invalid Dxpedition parse 1");
                callAcked = incoming[which++];
                if (callAcked.StartsWith("<"))
                {
                    callAckedIsHashed = true;
                    callAcked=callAcked.Substring(1);
                    if (callAcked.EndsWith(">"))
                        callAcked = callAcked.Substring(0, callAcked.Length - 1);
                }
                if (incoming.Length <= which)
                    throw new Exception("Invalid Dxpedition parse 2");
                if (incoming[which++] != "RR73;")
                    throw new Exception("Invalid Dxpedition parse 3");
                if (incoming.Length <= which)
                    throw new Exception("invalid Dxpedition parse 4");
                toCall = incoming[which++];
                if (toCall.StartsWith("<"))
                {
                    toCallIsHashed = true;
                    toCall = toCall.Substring(1);
                    if (toCall.EndsWith("<"))
                        toCall = toCall.Substring(0,toCall.Length - 1);
                }
                if (incoming.Length <= which)
                    throw new Exception("invalid Dxpedition parse 5");
                fromCall = incoming[which++];
                if (fromCall.StartsWith("<"))
                    fromCall = fromCall.Substring(1);
                if (fromCall.EndsWith(">"))
                    fromCall = fromCall.Substring(0, fromCall.Length - 1);
                if (incoming.Length <= which)
                    throw new Exception("invalid Dxpedition parse 6");
                report = incoming[which++];
            }

            public string ToCall { get { return toCall; }  }
            public bool ToCallIsHashed { get { return toCallIsHashed; } }
            public string Report { get { return report; }  }
            public string FromCall { get { return fromCall; }  }
            public bool FromCallIsHashed { get { return true; } }
            public string Exchange { get { return report; }  }
            public string RST { get { return null; } }
            public int SignaldB { get { return Int32.Parse(report); } }
            public string GridSquare { get { return null; } }
            public String CallQSLed { get { return callAcked; }  }
            public String QslText { get { return "RR73"; } }
            public bool CallQSLedIsHashed { get { return callAckedIsHashed; } }
        }

        public class StandardMessage : Message, ToFromCall, Roger, Exchange, QSL, IsCQ
        {
            // two calls, either one of which MAY be hasheed.
            //      followed by one of these:
            //          nothing
            //          RRR, 73, RR73
            //          grid
            //          R grid
            //          +/-nn
            //          R+/-nn
            // examples:
            // TO    FROM
            // TO is also allowed to be QRZ or DE, but otherwise must be ham call
            // W9XYZ K1ABC +03
            // W9XYZ K1ABC RRR
            // W9XYZ K1ABC em10
            // W9XYZ K1ABC R EM10
            // hash of non-standard calls
            // <PJ4/K1ABC> W9XYZ
            // W9XYZ <PJ4/K1ABC> +03
            // <PJ4/K1ABC> W9XYZ R-08
            // W9XYZ <PJ4/K1ABC> +03
            // W9XYZ <PJ4/K1ABC> RRR
            // W9XYZ <PJ4/K1ABC> em10
            // W9XYZ <PJ4/K1ABC> R EM10
            // <PJ4/K1ABC> W9XYZ R EM10
            // if begins with "CQ" then up to 4 letters can be between CQ and FROM

            private string toCall; // Is allowed to be QRZ or DE. Also CQ, in which case up to four letters can be appended
            private string fromCall; // toCall and fromCall may have /R appended. But no other slash anything
            private bool toCallIsHashed = false;
            private bool fromCallIsHashed = false;
            private bool includesAck = false;
            private string grid=null; // can also be "73" or "RRR"
            private string rpt;
            public StandardMessage(String[] incoming, int i3, int n3)
            {
                base.i3 = i3;
                base.n3 = n3;
                int which = 0;
                if (incoming.Length <= which)
                    throw new Exception("invalid StandardMessage parse 1");
                toCall = incoming[which++];
                if (which >= incoming.Length)
                    throw new Exception("invalid StandardMessage parse 2");
                if ((toCall == "CQ") && incoming[which].All(Char.IsLetter))
                {   // check for up to 4 letter "word" after the CQ
                    toCall += " ";
                    toCall += incoming[which++];
                    if (incoming.Length <= which)
                        throw new Exception("invalid StandardMessage parse 3");
                }
                if (toCall.StartsWith("<"))
                {
                    toCallIsHashed = true;
                    toCall = toCall.Substring(1);
                    if (toCall.EndsWith(">"))
                        toCall = toCall.Substring(0, toCall.Length-1);
                }
                fromCall = incoming[which++];
                if (fromCall.StartsWith("<"))
                {
                    fromCallIsHashed = true;
                    fromCall = fromCall.Substring(1);
                    if (fromCall.EndsWith(">"))
                        fromCall = fromCall.Substring(0, fromCall.Length - 1);
                }
                if (which < incoming.Length)
                {
                    int signPos = incoming[which].IndexOfAny("+-".ToCharArray());
                    if (signPos >= 0)
                    {
                        includesAck = incoming[which][0] == 'R';
                        if (includesAck)
                            rpt = incoming[which].Substring(1);
                        else
                            rpt = incoming[which];
                    }
                    else {
                        if (incoming[which] == "R")
                        {
                            includesAck = true;
                            which += 1;
                            if (incoming.Length <= which)
                                throw new Exception("invalid StandardMessage parse 4");
                        }
                        grid = incoming[which];
                    }
                }
            }

            public string FromCall { get { return fromCall;}  }
            public bool FromCallIsHashed { get { return fromCallIsHashed; } }
            public string ToCall { get { return toCall;}  }
            public bool ToCallIsHashed { get { return toCallIsHashed; } }
            public bool Roger { get { return includesAck;}  }
            public string Exchange { get {
                    if (!String.IsNullOrEmpty(grid))
                    {
                        if (null != CallQSLed)
                            return null; // those are not an "Exchange"
                        return grid;
                    }
                    if (!String.IsNullOrEmpty(rpt))
                        return rpt;
                    return null;
                    }  }
            public string RST { get { return null; } }
            public int SignaldB {
                get {
                    if (!String.IsNullOrEmpty(rpt))
                        return Int32.Parse(rpt);
                    return NO_DB;
                }
            }
            private bool isAcq {
                get { return toCall!=null && (
                        toCall == "CQ" || toCall.StartsWith("CQ ") || toCall == "DE" || toCall == "QRZ");  }
            }
            private bool isAcknowledge {
                get {return  grid == "RR73" || grid == "RRR" ||   grid == "73";}
            }
            public bool SolicitsAnswers { get { 
                    return isAcq || isAcknowledge;
                    } }
            public string GridSquare { get{ 
                    if (isAcknowledge) 
                        return null;
                    return grid; } }

            // interface QSL
            public string CallQSLed {
                get {
                    return isAcknowledge ? toCall :
                         (isAcq ? "ALL" : null);
                }
            }

            public string QslText { get { return isAcknowledge ? grid : null; } }
        }

        public class RttyRoundUpMessage : Message, ToFromCall, Exchange, Roger, QSL
        {
            // Roundup
            //  Optional TU;
            //  from call and to call
            //  optional R
            //  5n9 report (2 through 9)
            //  State or province or 1-7999 serial number
            // TU; W9XYZ K1ABC R 579 MA
            // hashed calls? 
            // BUG in WSJT-X 2.0.0 and 2.0.1:
            // the pack8 for RTTY round up fails to pack
            // the correct hash, but only if you do NOT but
            // angle brackets around the CALL
            // W1AW PJ2/K1ABC R 539 0005 -> W1AW <...> R 539 0005
            // PJ2/K1ABC W1AW R 539 0005 -> W1AW <...> R 539 0005
            bool containsTU;
            bool containsRoger = false;
            string toCall;
            bool toCallIsHashed = false;
            string fromCall;
            bool fromCallIsHashed = false;
            string rst="";
            string rpt="";
            public RttyRoundUpMessage(string[] incoming, int i3, int n3)
            {
                base.i3 = i3;
                base.n3 = n3;
                int which = 0;
                if (incoming.Length <= which)
                    throw new Exception("invalid RttyRoundUpMessage parse 1");
                containsTU = incoming[which] == "TU;";
                if (containsTU)
                {
                    which += 1;
                    if (incoming.Length <= which)
                        throw new Exception("invalid RttyRoundUpMessage parse 2");
                }
                toCall = incoming[which++];
                if (toCall.StartsWith("<"))
                {
                    toCallIsHashed = true;
                    toCall = toCall.Substring(1);
                    if (toCall.EndsWith(">"))
                        toCall = toCall.Substring(0, toCall.Length - 1);
                }
                if (incoming.Length <= which)
                    throw new Exception("invalid RttyRoundUpMessage parse 3");
                fromCall = incoming[which++];
                if (fromCall.StartsWith("<"))
                {
                    fromCallIsHashed = true;
                    fromCall = fromCall.Substring(1);
                    if (fromCall.EndsWith(">"))
                        fromCall = fromCall.Substring(0, fromCall.Length - 1);
                }
                if (incoming.Length <= which)
                    throw new Exception("invalid RttyRoundUpMessage parse 4");
                if (incoming[which] == "R")
                {
                    containsRoger = true;
                    which += 1;
                    if (incoming.Length <= which)
                        throw new Exception("invalid RttyRoundUpMessage parse 5");
                }
                rst = incoming[which++];
                if (incoming.Length <= which)
                    throw new Exception("invalid RttyRoundUpMessage parse 6");
                rpt = incoming[which];
            }

            public string FromCall { get { return fromCall;}  }
            public bool FromCallIsHashed { get { return fromCallIsHashed; } }
            public string ToCall { get { return toCall; }  }
            public bool ToCallIsHashed { get { return toCallIsHashed; } }
            public string Exchange { get { return rst + " " + rpt;}  }
            public string RST { get { return rst; } }
            public int SignaldB { get { return NO_DB; } }
            public String GridSquare { get { return null; } }
            public bool Roger { get { return containsRoger;}  }
            // interface QSL
            public String CallQSLed { get { return containsTU ? "ALL" :  null; }  }
            public String QslText { get { return containsTU ? "TU" :  null; } }

        }

        public class NonStandardCallMessage : Message, ToFromCall, QSL, IsCQ
        {
            // The non-standard CALL is the one NOT hashed
            // <W9XYZ> PJ4/K1ABC 
            // PJ4/K1ABC <W9XYZ> RRR
            // PJ4/K1ABC <W9XYZ> 73
            // CQ PJ4/KA1ABC
            string toCall;
            bool toCallIsHashed = false;
            string fromCall;
            bool fromCallIsHashed = false;
            string msg; // empty, or RR73 or RRR or 73
            public NonStandardCallMessage(string[] incoming, int i3, int n3)
            {
                base.i3 = i3;
                base.n3 = n3;
                int which = 0;
                if (incoming.Length <= which)
                    throw new Exception("invalid NonStandardCallMessage parse 1");
                toCall = incoming[which++];
                if (toCall.StartsWith("<"))
                {
                    toCallIsHashed = true;
                    toCall = toCall.Substring(1);
                    if (toCall.EndsWith(">"))
                        toCall = toCall.Substring(0, toCall.Length - 1);
                }
                if (incoming.Length <= which)
                    throw new Exception("invalid NonStandardCallMessage parse 2");
                fromCall = incoming[which++];
                if (fromCall.StartsWith("<"))
                {
                    fromCallIsHashed = true;
                    fromCall = fromCall.Substring(1);
                    if (fromCall.EndsWith(">"))
                        fromCall = fromCall.Substring(0, fromCall.Length - 1);
                }
                if (which < incoming.Length)
                    msg = incoming[which];
            }

            public string FromCall { get { return fromCall;}  }
            public bool FromCallIsHashed { get { return fromCallIsHashed; } }
            public string ToCall { get { return toCall;}  }
            public bool ToCallIsHashed { get { return toCallIsHashed; } }
            public bool SolicitsAnswers { get { return !String.IsNullOrEmpty(CallQSLed);} }       
            private bool startWithCQ { get { return null != toCall && (toCall.StartsWith("CQ ") || toCall == "CQ" );} }
            // interface QSL
            public string CallQSLed { get { return 
                        startWithCQ ? "ALL" :(
                            msg == "RRR" || msg == "RR73" ||
                            msg == "73" ? toCall : null);}  }
            public String QslText { get { return msg; } }
        }

        public class ArrlFieldDayMessage : Message, ToFromCall, Roger, Exchange
        {
            // WA9XYZ KA1ABC R 16A EMA
            string fromCall;
            bool fromCallIsHashed = false;
            string toCall;
            bool toCallIsHashed = false;
            string classifer;
            string section;
            bool containsRoger = false;
            public ArrlFieldDayMessage(string[] incoming, int i3, int n3)
            {
                base.i3 = i3;
                base.n3 = n3;
                int which = 0;
                if (incoming.Length <= which)
                    throw new Exception("invalid ArrlFieldDayMessage parse 1");
                toCall = incoming[which++];
                if (toCall.StartsWith("<"))
                {
                    toCallIsHashed = true;
                    toCall = toCall.Substring(1);
                    if (toCall.EndsWith(">"))
                        toCall = toCall.Substring(0, toCall.Length - 1);
                }
                if (incoming.Length <= which)
                    throw new Exception("invalid ArrlFieldDayMessage parse 2");
                fromCall = incoming[which++];
                if (fromCall.StartsWith("<"))
                {
                    fromCallIsHashed = true;
                    fromCall = fromCall.Substring(1);
                    if (fromCall.EndsWith(">"))
                        fromCall = fromCall.Substring(0, fromCall.Length - 1);
                }
                if (incoming.Length <= which)
                    throw new Exception("invalid ArrlFieldDayMessage parse 3");
                if (incoming[which] == "R")
                {
                    containsRoger = true;
                    which += 1;
                    if (incoming.Length <= which)
                        throw new Exception("invalid ArrlFieldDayMessage parse 4");
                }
                classifer = incoming[which++];
                if (incoming.Length <= which)
                    throw new Exception("invalid ArrlFieldDayMessage parse 5");
                section = incoming[which++];
            }

            public string FromCall { get { return fromCall;}  }
            public bool FromCallIsHashed { get { return fromCallIsHashed; } }
            public string ToCall { get { return toCall;}  }
            public bool ToCallIsHashed { get { return toCallIsHashed; } }
            public bool Roger { get {return containsRoger; }  }
            public string Exchange { get { return classifer + " " + section;}  }
            public string RST { get { return null; } }
            public int SignaldB { get { return NO_DB; } }
            public string GridSquare { get { return null; } }
        }

    }
}
