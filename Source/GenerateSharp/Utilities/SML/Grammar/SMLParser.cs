//------------------------------------------------------------------------------
// <auto-generated>
//     This code was generated by a tool.
//     ANTLR Version: 4.11.1
//
//     Changes to this file may cause incorrect behavior and will be lost if
//     the code is regenerated.
// </auto-generated>
//------------------------------------------------------------------------------

// Generated from SML.g4 by ANTLR 4.11.1

// Unreachable code detected
#pragma warning disable 0162
// The variable '...' is assigned but its value is never used
#pragma warning disable 0219
// Missing XML comment for publicly visible type or member '...'
#pragma warning disable 1591
// Ambiguous reference in cref attribute
#pragma warning disable 419

using System;
using System.IO;
using System.Text;
using System.Diagnostics;
using System.Collections.Generic;
using Antlr4.Runtime;
using Antlr4.Runtime.Atn;
using Antlr4.Runtime.Misc;
using Antlr4.Runtime.Tree;
using DFA = Antlr4.Runtime.Dfa.DFA;

[System.CodeDom.Compiler.GeneratedCode("ANTLR", "4.11.1")]
[System.CLSCompliant(false)]
public partial class SMLParser : Parser {
	protected static DFA[] decisionToDFA;
	protected static PredictionContextCache sharedContextCache = new PredictionContextCache();
	public const int
		COLON=1, OPEN_BRACKET=2, CLOSE_BRACKET=3, OPEN_BRACE=4, CLOSE_BRACE=5, 
		COMMA=6, TRUE=7, FALSE=8, NEWLINE=9, COMMENT=10, INTEGER=11, KEY=12, WORD=13, 
		STRING_LITERAL=14, WHITESPACE=15;
	public const int
		RULE_document = 0, RULE_table = 1, RULE_tableContent = 2, RULE_tableValue = 3, 
		RULE_array = 4, RULE_arrayContent = 5, RULE_value = 6, RULE_delimiter = 7;
	public static readonly string[] ruleNames = {
		"document", "table", "tableContent", "tableValue", "array", "arrayContent", 
		"value", "delimiter"
	};

	private static readonly string[] _LiteralNames = {
		null, "':'", "'['", "']'", "'{'", "'}'", "','", "'true'", "'false'"
	};
	private static readonly string[] _SymbolicNames = {
		null, "COLON", "OPEN_BRACKET", "CLOSE_BRACKET", "OPEN_BRACE", "CLOSE_BRACE", 
		"COMMA", "TRUE", "FALSE", "NEWLINE", "COMMENT", "INTEGER", "KEY", "WORD", 
		"STRING_LITERAL", "WHITESPACE"
	};
	public static readonly IVocabulary DefaultVocabulary = new Vocabulary(_LiteralNames, _SymbolicNames);

	[NotNull]
	public override IVocabulary Vocabulary
	{
		get
		{
			return DefaultVocabulary;
		}
	}

	public override string GrammarFileName { get { return "SML.g4"; } }

	public override string[] RuleNames { get { return ruleNames; } }

	public override int[] SerializedAtn { get { return _serializedATN; } }

	static SMLParser() {
		decisionToDFA = new DFA[_ATN.NumberOfDecisions];
		for (int i = 0; i < _ATN.NumberOfDecisions; i++) {
			decisionToDFA[i] = new DFA(_ATN.GetDecisionState(i), i);
		}
	}

		public SMLParser(ITokenStream input) : this(input, Console.Out, Console.Error) { }

		public SMLParser(ITokenStream input, TextWriter output, TextWriter errorOutput)
		: base(input, output, errorOutput)
	{
		Interpreter = new ParserATNSimulator(this, _ATN, decisionToDFA, sharedContextCache);
	}

	public partial class DocumentContext : ParserRuleContext {
		[System.Diagnostics.DebuggerNonUserCode] public TableContentContext tableContent() {
			return GetRuleContext<TableContentContext>(0);
		}
		[System.Diagnostics.DebuggerNonUserCode] public ITerminalNode Eof() { return GetToken(SMLParser.Eof, 0); }
		public DocumentContext(ParserRuleContext parent, int invokingState)
			: base(parent, invokingState)
		{
		}
		public override int RuleIndex { get { return RULE_document; } }
		[System.Diagnostics.DebuggerNonUserCode]
		public override TResult Accept<TResult>(IParseTreeVisitor<TResult> visitor) {
			ISMLVisitor<TResult> typedVisitor = visitor as ISMLVisitor<TResult>;
			if (typedVisitor != null) return typedVisitor.VisitDocument(this);
			else return visitor.VisitChildren(this);
		}
	}

	[RuleVersion(0)]
	public DocumentContext document() {
		DocumentContext _localctx = new DocumentContext(Context, State);
		EnterRule(_localctx, 0, RULE_document);
		try {
			EnterOuterAlt(_localctx, 1);
			{
			State = 16;
			tableContent();
			State = 17;
			Match(Eof);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			ErrorHandler.ReportError(this, re);
			ErrorHandler.Recover(this, re);
		}
		finally {
			ExitRule();
		}
		return _localctx;
	}

	public partial class TableContext : ParserRuleContext {
		[System.Diagnostics.DebuggerNonUserCode] public ITerminalNode OPEN_BRACE() { return GetToken(SMLParser.OPEN_BRACE, 0); }
		[System.Diagnostics.DebuggerNonUserCode] public TableContentContext tableContent() {
			return GetRuleContext<TableContentContext>(0);
		}
		[System.Diagnostics.DebuggerNonUserCode] public ITerminalNode CLOSE_BRACE() { return GetToken(SMLParser.CLOSE_BRACE, 0); }
		public TableContext(ParserRuleContext parent, int invokingState)
			: base(parent, invokingState)
		{
		}
		public override int RuleIndex { get { return RULE_table; } }
		[System.Diagnostics.DebuggerNonUserCode]
		public override TResult Accept<TResult>(IParseTreeVisitor<TResult> visitor) {
			ISMLVisitor<TResult> typedVisitor = visitor as ISMLVisitor<TResult>;
			if (typedVisitor != null) return typedVisitor.VisitTable(this);
			else return visitor.VisitChildren(this);
		}
	}

	[RuleVersion(0)]
	public TableContext table() {
		TableContext _localctx = new TableContext(Context, State);
		EnterRule(_localctx, 2, RULE_table);
		try {
			EnterOuterAlt(_localctx, 1);
			{
			State = 19;
			Match(OPEN_BRACE);
			State = 20;
			tableContent();
			State = 21;
			Match(CLOSE_BRACE);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			ErrorHandler.ReportError(this, re);
			ErrorHandler.Recover(this, re);
		}
		finally {
			ExitRule();
		}
		return _localctx;
	}

	public partial class TableContentContext : ParserRuleContext {
		[System.Diagnostics.DebuggerNonUserCode] public ITerminalNode[] NEWLINE() { return GetTokens(SMLParser.NEWLINE); }
		[System.Diagnostics.DebuggerNonUserCode] public ITerminalNode NEWLINE(int i) {
			return GetToken(SMLParser.NEWLINE, i);
		}
		[System.Diagnostics.DebuggerNonUserCode] public TableValueContext[] tableValue() {
			return GetRuleContexts<TableValueContext>();
		}
		[System.Diagnostics.DebuggerNonUserCode] public TableValueContext tableValue(int i) {
			return GetRuleContext<TableValueContext>(i);
		}
		[System.Diagnostics.DebuggerNonUserCode] public DelimiterContext[] delimiter() {
			return GetRuleContexts<DelimiterContext>();
		}
		[System.Diagnostics.DebuggerNonUserCode] public DelimiterContext delimiter(int i) {
			return GetRuleContext<DelimiterContext>(i);
		}
		public TableContentContext(ParserRuleContext parent, int invokingState)
			: base(parent, invokingState)
		{
		}
		public override int RuleIndex { get { return RULE_tableContent; } }
		[System.Diagnostics.DebuggerNonUserCode]
		public override TResult Accept<TResult>(IParseTreeVisitor<TResult> visitor) {
			ISMLVisitor<TResult> typedVisitor = visitor as ISMLVisitor<TResult>;
			if (typedVisitor != null) return typedVisitor.VisitTableContent(this);
			else return visitor.VisitChildren(this);
		}
	}

	[RuleVersion(0)]
	public TableContentContext tableContent() {
		TableContentContext _localctx = new TableContentContext(Context, State);
		EnterRule(_localctx, 4, RULE_tableContent);
		int _la;
		try {
			int _alt;
			EnterOuterAlt(_localctx, 1);
			{
			State = 26;
			ErrorHandler.Sync(this);
			_la = TokenStream.LA(1);
			while (_la==NEWLINE) {
				{
				{
				State = 23;
				Match(NEWLINE);
				}
				}
				State = 28;
				ErrorHandler.Sync(this);
				_la = TokenStream.LA(1);
			}
			State = 44;
			ErrorHandler.Sync(this);
			_la = TokenStream.LA(1);
			if (_la==KEY) {
				{
				State = 29;
				tableValue();
				State = 35;
				ErrorHandler.Sync(this);
				_alt = Interpreter.AdaptivePredict(TokenStream,1,Context);
				while ( _alt!=2 && _alt!=global::Antlr4.Runtime.Atn.ATN.INVALID_ALT_NUMBER ) {
					if ( _alt==1 ) {
						{
						{
						State = 30;
						delimiter();
						State = 31;
						tableValue();
						}
						} 
					}
					State = 37;
					ErrorHandler.Sync(this);
					_alt = Interpreter.AdaptivePredict(TokenStream,1,Context);
				}
				State = 41;
				ErrorHandler.Sync(this);
				_la = TokenStream.LA(1);
				while (_la==NEWLINE) {
					{
					{
					State = 38;
					Match(NEWLINE);
					}
					}
					State = 43;
					ErrorHandler.Sync(this);
					_la = TokenStream.LA(1);
				}
				}
			}

			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			ErrorHandler.ReportError(this, re);
			ErrorHandler.Recover(this, re);
		}
		finally {
			ExitRule();
		}
		return _localctx;
	}

	public partial class TableValueContext : ParserRuleContext {
		[System.Diagnostics.DebuggerNonUserCode] public ITerminalNode KEY() { return GetToken(SMLParser.KEY, 0); }
		[System.Diagnostics.DebuggerNonUserCode] public ITerminalNode COLON() { return GetToken(SMLParser.COLON, 0); }
		[System.Diagnostics.DebuggerNonUserCode] public ValueContext value() {
			return GetRuleContext<ValueContext>(0);
		}
		public TableValueContext(ParserRuleContext parent, int invokingState)
			: base(parent, invokingState)
		{
		}
		public override int RuleIndex { get { return RULE_tableValue; } }
		[System.Diagnostics.DebuggerNonUserCode]
		public override TResult Accept<TResult>(IParseTreeVisitor<TResult> visitor) {
			ISMLVisitor<TResult> typedVisitor = visitor as ISMLVisitor<TResult>;
			if (typedVisitor != null) return typedVisitor.VisitTableValue(this);
			else return visitor.VisitChildren(this);
		}
	}

	[RuleVersion(0)]
	public TableValueContext tableValue() {
		TableValueContext _localctx = new TableValueContext(Context, State);
		EnterRule(_localctx, 6, RULE_tableValue);
		try {
			EnterOuterAlt(_localctx, 1);
			{
			State = 46;
			Match(KEY);
			State = 47;
			Match(COLON);
			State = 48;
			value();
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			ErrorHandler.ReportError(this, re);
			ErrorHandler.Recover(this, re);
		}
		finally {
			ExitRule();
		}
		return _localctx;
	}

	public partial class ArrayContext : ParserRuleContext {
		[System.Diagnostics.DebuggerNonUserCode] public ITerminalNode OPEN_BRACKET() { return GetToken(SMLParser.OPEN_BRACKET, 0); }
		[System.Diagnostics.DebuggerNonUserCode] public ArrayContentContext arrayContent() {
			return GetRuleContext<ArrayContentContext>(0);
		}
		[System.Diagnostics.DebuggerNonUserCode] public ITerminalNode CLOSE_BRACKET() { return GetToken(SMLParser.CLOSE_BRACKET, 0); }
		public ArrayContext(ParserRuleContext parent, int invokingState)
			: base(parent, invokingState)
		{
		}
		public override int RuleIndex { get { return RULE_array; } }
		[System.Diagnostics.DebuggerNonUserCode]
		public override TResult Accept<TResult>(IParseTreeVisitor<TResult> visitor) {
			ISMLVisitor<TResult> typedVisitor = visitor as ISMLVisitor<TResult>;
			if (typedVisitor != null) return typedVisitor.VisitArray(this);
			else return visitor.VisitChildren(this);
		}
	}

	[RuleVersion(0)]
	public ArrayContext array() {
		ArrayContext _localctx = new ArrayContext(Context, State);
		EnterRule(_localctx, 8, RULE_array);
		try {
			EnterOuterAlt(_localctx, 1);
			{
			State = 50;
			Match(OPEN_BRACKET);
			State = 51;
			arrayContent();
			State = 52;
			Match(CLOSE_BRACKET);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			ErrorHandler.ReportError(this, re);
			ErrorHandler.Recover(this, re);
		}
		finally {
			ExitRule();
		}
		return _localctx;
	}

	public partial class ArrayContentContext : ParserRuleContext {
		[System.Diagnostics.DebuggerNonUserCode] public ITerminalNode[] NEWLINE() { return GetTokens(SMLParser.NEWLINE); }
		[System.Diagnostics.DebuggerNonUserCode] public ITerminalNode NEWLINE(int i) {
			return GetToken(SMLParser.NEWLINE, i);
		}
		[System.Diagnostics.DebuggerNonUserCode] public ValueContext[] value() {
			return GetRuleContexts<ValueContext>();
		}
		[System.Diagnostics.DebuggerNonUserCode] public ValueContext value(int i) {
			return GetRuleContext<ValueContext>(i);
		}
		[System.Diagnostics.DebuggerNonUserCode] public DelimiterContext[] delimiter() {
			return GetRuleContexts<DelimiterContext>();
		}
		[System.Diagnostics.DebuggerNonUserCode] public DelimiterContext delimiter(int i) {
			return GetRuleContext<DelimiterContext>(i);
		}
		public ArrayContentContext(ParserRuleContext parent, int invokingState)
			: base(parent, invokingState)
		{
		}
		public override int RuleIndex { get { return RULE_arrayContent; } }
		[System.Diagnostics.DebuggerNonUserCode]
		public override TResult Accept<TResult>(IParseTreeVisitor<TResult> visitor) {
			ISMLVisitor<TResult> typedVisitor = visitor as ISMLVisitor<TResult>;
			if (typedVisitor != null) return typedVisitor.VisitArrayContent(this);
			else return visitor.VisitChildren(this);
		}
	}

	[RuleVersion(0)]
	public ArrayContentContext arrayContent() {
		ArrayContentContext _localctx = new ArrayContentContext(Context, State);
		EnterRule(_localctx, 10, RULE_arrayContent);
		int _la;
		try {
			int _alt;
			EnterOuterAlt(_localctx, 1);
			{
			State = 57;
			ErrorHandler.Sync(this);
			_la = TokenStream.LA(1);
			while (_la==NEWLINE) {
				{
				{
				State = 54;
				Match(NEWLINE);
				}
				}
				State = 59;
				ErrorHandler.Sync(this);
				_la = TokenStream.LA(1);
			}
			State = 75;
			ErrorHandler.Sync(this);
			_la = TokenStream.LA(1);
			if (((_la) & ~0x3f) == 0 && ((1L << _la) & 18836L) != 0) {
				{
				State = 60;
				value();
				State = 66;
				ErrorHandler.Sync(this);
				_alt = Interpreter.AdaptivePredict(TokenStream,5,Context);
				while ( _alt!=2 && _alt!=global::Antlr4.Runtime.Atn.ATN.INVALID_ALT_NUMBER ) {
					if ( _alt==1 ) {
						{
						{
						State = 61;
						delimiter();
						State = 62;
						value();
						}
						} 
					}
					State = 68;
					ErrorHandler.Sync(this);
					_alt = Interpreter.AdaptivePredict(TokenStream,5,Context);
				}
				State = 72;
				ErrorHandler.Sync(this);
				_la = TokenStream.LA(1);
				while (_la==NEWLINE) {
					{
					{
					State = 69;
					Match(NEWLINE);
					}
					}
					State = 74;
					ErrorHandler.Sync(this);
					_la = TokenStream.LA(1);
				}
				}
			}

			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			ErrorHandler.ReportError(this, re);
			ErrorHandler.Recover(this, re);
		}
		finally {
			ExitRule();
		}
		return _localctx;
	}

	public partial class ValueContext : ParserRuleContext {
		public ValueContext(ParserRuleContext parent, int invokingState)
			: base(parent, invokingState)
		{
		}
		public override int RuleIndex { get { return RULE_value; } }
	 
		public ValueContext() { }
		public virtual void CopyFrom(ValueContext context) {
			base.CopyFrom(context);
		}
	}
	public partial class ValueTrueContext : ValueContext {
		[System.Diagnostics.DebuggerNonUserCode] public ITerminalNode TRUE() { return GetToken(SMLParser.TRUE, 0); }
		public ValueTrueContext(ValueContext context) { CopyFrom(context); }
		[System.Diagnostics.DebuggerNonUserCode]
		public override TResult Accept<TResult>(IParseTreeVisitor<TResult> visitor) {
			ISMLVisitor<TResult> typedVisitor = visitor as ISMLVisitor<TResult>;
			if (typedVisitor != null) return typedVisitor.VisitValueTrue(this);
			else return visitor.VisitChildren(this);
		}
	}
	public partial class ValueFalseContext : ValueContext {
		[System.Diagnostics.DebuggerNonUserCode] public ITerminalNode FALSE() { return GetToken(SMLParser.FALSE, 0); }
		public ValueFalseContext(ValueContext context) { CopyFrom(context); }
		[System.Diagnostics.DebuggerNonUserCode]
		public override TResult Accept<TResult>(IParseTreeVisitor<TResult> visitor) {
			ISMLVisitor<TResult> typedVisitor = visitor as ISMLVisitor<TResult>;
			if (typedVisitor != null) return typedVisitor.VisitValueFalse(this);
			else return visitor.VisitChildren(this);
		}
	}
	public partial class ValueStringContext : ValueContext {
		[System.Diagnostics.DebuggerNonUserCode] public ITerminalNode STRING_LITERAL() { return GetToken(SMLParser.STRING_LITERAL, 0); }
		public ValueStringContext(ValueContext context) { CopyFrom(context); }
		[System.Diagnostics.DebuggerNonUserCode]
		public override TResult Accept<TResult>(IParseTreeVisitor<TResult> visitor) {
			ISMLVisitor<TResult> typedVisitor = visitor as ISMLVisitor<TResult>;
			if (typedVisitor != null) return typedVisitor.VisitValueString(this);
			else return visitor.VisitChildren(this);
		}
	}
	public partial class ValueTableContext : ValueContext {
		[System.Diagnostics.DebuggerNonUserCode] public TableContext table() {
			return GetRuleContext<TableContext>(0);
		}
		public ValueTableContext(ValueContext context) { CopyFrom(context); }
		[System.Diagnostics.DebuggerNonUserCode]
		public override TResult Accept<TResult>(IParseTreeVisitor<TResult> visitor) {
			ISMLVisitor<TResult> typedVisitor = visitor as ISMLVisitor<TResult>;
			if (typedVisitor != null) return typedVisitor.VisitValueTable(this);
			else return visitor.VisitChildren(this);
		}
	}
	public partial class ValueArrayContext : ValueContext {
		[System.Diagnostics.DebuggerNonUserCode] public ArrayContext array() {
			return GetRuleContext<ArrayContext>(0);
		}
		public ValueArrayContext(ValueContext context) { CopyFrom(context); }
		[System.Diagnostics.DebuggerNonUserCode]
		public override TResult Accept<TResult>(IParseTreeVisitor<TResult> visitor) {
			ISMLVisitor<TResult> typedVisitor = visitor as ISMLVisitor<TResult>;
			if (typedVisitor != null) return typedVisitor.VisitValueArray(this);
			else return visitor.VisitChildren(this);
		}
	}
	public partial class ValueIntegerContext : ValueContext {
		[System.Diagnostics.DebuggerNonUserCode] public ITerminalNode INTEGER() { return GetToken(SMLParser.INTEGER, 0); }
		public ValueIntegerContext(ValueContext context) { CopyFrom(context); }
		[System.Diagnostics.DebuggerNonUserCode]
		public override TResult Accept<TResult>(IParseTreeVisitor<TResult> visitor) {
			ISMLVisitor<TResult> typedVisitor = visitor as ISMLVisitor<TResult>;
			if (typedVisitor != null) return typedVisitor.VisitValueInteger(this);
			else return visitor.VisitChildren(this);
		}
	}

	[RuleVersion(0)]
	public ValueContext value() {
		ValueContext _localctx = new ValueContext(Context, State);
		EnterRule(_localctx, 12, RULE_value);
		try {
			State = 83;
			ErrorHandler.Sync(this);
			switch (TokenStream.LA(1)) {
			case INTEGER:
				_localctx = new ValueIntegerContext(_localctx);
				EnterOuterAlt(_localctx, 1);
				{
				State = 77;
				Match(INTEGER);
				}
				break;
			case STRING_LITERAL:
				_localctx = new ValueStringContext(_localctx);
				EnterOuterAlt(_localctx, 2);
				{
				State = 78;
				Match(STRING_LITERAL);
				}
				break;
			case TRUE:
				_localctx = new ValueTrueContext(_localctx);
				EnterOuterAlt(_localctx, 3);
				{
				State = 79;
				Match(TRUE);
				}
				break;
			case FALSE:
				_localctx = new ValueFalseContext(_localctx);
				EnterOuterAlt(_localctx, 4);
				{
				State = 80;
				Match(FALSE);
				}
				break;
			case OPEN_BRACE:
				_localctx = new ValueTableContext(_localctx);
				EnterOuterAlt(_localctx, 5);
				{
				State = 81;
				table();
				}
				break;
			case OPEN_BRACKET:
				_localctx = new ValueArrayContext(_localctx);
				EnterOuterAlt(_localctx, 6);
				{
				State = 82;
				array();
				}
				break;
			default:
				throw new NoViableAltException(this);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			ErrorHandler.ReportError(this, re);
			ErrorHandler.Recover(this, re);
		}
		finally {
			ExitRule();
		}
		return _localctx;
	}

	public partial class DelimiterContext : ParserRuleContext {
		public DelimiterContext(ParserRuleContext parent, int invokingState)
			: base(parent, invokingState)
		{
		}
		public override int RuleIndex { get { return RULE_delimiter; } }
	 
		public DelimiterContext() { }
		public virtual void CopyFrom(DelimiterContext context) {
			base.CopyFrom(context);
		}
	}
	public partial class CommaDelimiterContext : DelimiterContext {
		[System.Diagnostics.DebuggerNonUserCode] public ITerminalNode COMMA() { return GetToken(SMLParser.COMMA, 0); }
		public CommaDelimiterContext(DelimiterContext context) { CopyFrom(context); }
		[System.Diagnostics.DebuggerNonUserCode]
		public override TResult Accept<TResult>(IParseTreeVisitor<TResult> visitor) {
			ISMLVisitor<TResult> typedVisitor = visitor as ISMLVisitor<TResult>;
			if (typedVisitor != null) return typedVisitor.VisitCommaDelimiter(this);
			else return visitor.VisitChildren(this);
		}
	}
	public partial class NewlineDelimiterContext : DelimiterContext {
		[System.Diagnostics.DebuggerNonUserCode] public ITerminalNode[] NEWLINE() { return GetTokens(SMLParser.NEWLINE); }
		[System.Diagnostics.DebuggerNonUserCode] public ITerminalNode NEWLINE(int i) {
			return GetToken(SMLParser.NEWLINE, i);
		}
		public NewlineDelimiterContext(DelimiterContext context) { CopyFrom(context); }
		[System.Diagnostics.DebuggerNonUserCode]
		public override TResult Accept<TResult>(IParseTreeVisitor<TResult> visitor) {
			ISMLVisitor<TResult> typedVisitor = visitor as ISMLVisitor<TResult>;
			if (typedVisitor != null) return typedVisitor.VisitNewlineDelimiter(this);
			else return visitor.VisitChildren(this);
		}
	}

	[RuleVersion(0)]
	public DelimiterContext delimiter() {
		DelimiterContext _localctx = new DelimiterContext(Context, State);
		EnterRule(_localctx, 14, RULE_delimiter);
		int _la;
		try {
			State = 91;
			ErrorHandler.Sync(this);
			switch (TokenStream.LA(1)) {
			case NEWLINE:
				_localctx = new NewlineDelimiterContext(_localctx);
				EnterOuterAlt(_localctx, 1);
				{
				State = 86;
				ErrorHandler.Sync(this);
				_la = TokenStream.LA(1);
				do {
					{
					{
					State = 85;
					Match(NEWLINE);
					}
					}
					State = 88;
					ErrorHandler.Sync(this);
					_la = TokenStream.LA(1);
				} while ( _la==NEWLINE );
				}
				break;
			case COMMA:
				_localctx = new CommaDelimiterContext(_localctx);
				EnterOuterAlt(_localctx, 2);
				{
				State = 90;
				Match(COMMA);
				}
				break;
			default:
				throw new NoViableAltException(this);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			ErrorHandler.ReportError(this, re);
			ErrorHandler.Recover(this, re);
		}
		finally {
			ExitRule();
		}
		return _localctx;
	}

	private static int[] _serializedATN = {
		4,1,15,94,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,6,2,7,
		7,7,1,0,1,0,1,0,1,1,1,1,1,1,1,1,1,2,5,2,25,8,2,10,2,12,2,28,9,2,1,2,1,
		2,1,2,1,2,5,2,34,8,2,10,2,12,2,37,9,2,1,2,5,2,40,8,2,10,2,12,2,43,9,2,
		3,2,45,8,2,1,3,1,3,1,3,1,3,1,4,1,4,1,4,1,4,1,5,5,5,56,8,5,10,5,12,5,59,
		9,5,1,5,1,5,1,5,1,5,5,5,65,8,5,10,5,12,5,68,9,5,1,5,5,5,71,8,5,10,5,12,
		5,74,9,5,3,5,76,8,5,1,6,1,6,1,6,1,6,1,6,1,6,3,6,84,8,6,1,7,4,7,87,8,7,
		11,7,12,7,88,1,7,3,7,92,8,7,1,7,0,0,8,0,2,4,6,8,10,12,14,0,0,100,0,16,
		1,0,0,0,2,19,1,0,0,0,4,26,1,0,0,0,6,46,1,0,0,0,8,50,1,0,0,0,10,57,1,0,
		0,0,12,83,1,0,0,0,14,91,1,0,0,0,16,17,3,4,2,0,17,18,5,0,0,1,18,1,1,0,0,
		0,19,20,5,4,0,0,20,21,3,4,2,0,21,22,5,5,0,0,22,3,1,0,0,0,23,25,5,9,0,0,
		24,23,1,0,0,0,25,28,1,0,0,0,26,24,1,0,0,0,26,27,1,0,0,0,27,44,1,0,0,0,
		28,26,1,0,0,0,29,35,3,6,3,0,30,31,3,14,7,0,31,32,3,6,3,0,32,34,1,0,0,0,
		33,30,1,0,0,0,34,37,1,0,0,0,35,33,1,0,0,0,35,36,1,0,0,0,36,41,1,0,0,0,
		37,35,1,0,0,0,38,40,5,9,0,0,39,38,1,0,0,0,40,43,1,0,0,0,41,39,1,0,0,0,
		41,42,1,0,0,0,42,45,1,0,0,0,43,41,1,0,0,0,44,29,1,0,0,0,44,45,1,0,0,0,
		45,5,1,0,0,0,46,47,5,12,0,0,47,48,5,1,0,0,48,49,3,12,6,0,49,7,1,0,0,0,
		50,51,5,2,0,0,51,52,3,10,5,0,52,53,5,3,0,0,53,9,1,0,0,0,54,56,5,9,0,0,
		55,54,1,0,0,0,56,59,1,0,0,0,57,55,1,0,0,0,57,58,1,0,0,0,58,75,1,0,0,0,
		59,57,1,0,0,0,60,66,3,12,6,0,61,62,3,14,7,0,62,63,3,12,6,0,63,65,1,0,0,
		0,64,61,1,0,0,0,65,68,1,0,0,0,66,64,1,0,0,0,66,67,1,0,0,0,67,72,1,0,0,
		0,68,66,1,0,0,0,69,71,5,9,0,0,70,69,1,0,0,0,71,74,1,0,0,0,72,70,1,0,0,
		0,72,73,1,0,0,0,73,76,1,0,0,0,74,72,1,0,0,0,75,60,1,0,0,0,75,76,1,0,0,
		0,76,11,1,0,0,0,77,84,5,11,0,0,78,84,5,14,0,0,79,84,5,7,0,0,80,84,5,8,
		0,0,81,84,3,2,1,0,82,84,3,8,4,0,83,77,1,0,0,0,83,78,1,0,0,0,83,79,1,0,
		0,0,83,80,1,0,0,0,83,81,1,0,0,0,83,82,1,0,0,0,84,13,1,0,0,0,85,87,5,9,
		0,0,86,85,1,0,0,0,87,88,1,0,0,0,88,86,1,0,0,0,88,89,1,0,0,0,89,92,1,0,
		0,0,90,92,5,6,0,0,91,86,1,0,0,0,91,90,1,0,0,0,92,15,1,0,0,0,11,26,35,41,
		44,57,66,72,75,83,88,91
	};

	public static readonly ATN _ATN =
		new ATNDeserializer().Deserialize(_serializedATN);


}
