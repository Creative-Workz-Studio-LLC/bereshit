package agent

import (
	"testing"
)

// --- OutputFormat ---

func TestOutputFormat_String(t *testing.T) {
	tests := []struct {
		format OutputFormat
		want   string
	}{
		{FormatText, "text"},
		{FormatJSON, "json"},
		{FormatJSONArray, "json_array"},
		{FormatList, "list"},
		{FormatKeyValue, "key_value"},
		{FormatBool, "bool"},
		{OutputFormat(99), "unknown"},
	}

	for _, tt := range tests {
		if got := tt.format.String(); got != tt.want {
			t.Errorf("OutputFormat(%d).String() = %q, want %q", tt.format, got, tt.want)
		}
	}
}

// --- FormatPrompt ---

func TestFormatPrompt_Text(t *testing.T) {
	prompt := "What is the weather?"
	result := FormatPrompt(prompt, OutputSchema{Format: FormatText})
	if result != prompt {
		t.Errorf("text format should return prompt unchanged")
	}
}

func TestFormatPrompt_JSON(t *testing.T) {
	result := FormatPrompt("Get user info", OutputSchema{
		Format: FormatJSON,
		Fields: []SchemaField{
			{Name: "name", Type: "string", Required: true, Description: "user's name"},
			{Name: "age", Type: "number"},
		},
	})

	if !containsStr(result, "JSON object") {
		t.Error("should mention JSON object")
	}
	if !containsStr(result, `"name"`) {
		t.Error("should mention name field")
	}
	if !containsStr(result, "(required)") {
		t.Error("should mark required field")
	}
}

func TestFormatPrompt_JSONNoFields(t *testing.T) {
	result := FormatPrompt("Get data", OutputSchema{Format: FormatJSON})
	if !containsStr(result, "JSON object") {
		t.Error("should mention JSON object")
	}
}

func TestFormatPrompt_JSONArray(t *testing.T) {
	result := FormatPrompt("List items", OutputSchema{Format: FormatJSONArray})
	if !containsStr(result, "JSON array") {
		t.Error("should mention JSON array")
	}
}

func TestFormatPrompt_List(t *testing.T) {
	result := FormatPrompt("List things", OutputSchema{Format: FormatList})
	if !containsStr(result, "one item per line") {
		t.Error("should mention list format")
	}
}

func TestFormatPrompt_KeyValue(t *testing.T) {
	result := FormatPrompt("Get info", OutputSchema{Format: FormatKeyValue})
	if !containsStr(result, "key: value") {
		t.Error("should mention key-value format")
	}
}

func TestFormatPrompt_Bool(t *testing.T) {
	result := FormatPrompt("Is it true?", OutputSchema{Format: FormatBool})
	if !containsStr(result, "yes") || !containsStr(result, "no") {
		t.Error("should mention yes/no")
	}
}

func TestFormatPrompt_WithDescription(t *testing.T) {
	result := FormatPrompt("Get data", OutputSchema{
		Format:      FormatJSON,
		Description: "Include all fields from the database.",
	})
	if !containsStr(result, "Include all fields") {
		t.Error("should include description")
	}
}

func TestFormatPrompt_WithExample(t *testing.T) {
	result := FormatPrompt("Get data", OutputSchema{
		Format:  FormatJSON,
		Example: `{"name": "test"}`,
	})
	if !containsStr(result, `{"name": "test"}`) {
		t.Error("should include example")
	}
}

// --- ParseResponse ---

func TestParseResponse_Text(t *testing.T) {
	result, err := ParseResponse("hello world", FormatText)
	if err != nil {
		t.Fatal(err)
	}
	if result.Raw != "hello world" {
		t.Error("raw should be preserved")
	}
}

func TestParseResponse_JSON(t *testing.T) {
	result, err := ParseResponse(`{"name": "Nova", "age": 1}`, FormatJSON)
	if err != nil {
		t.Fatal(err)
	}
	data, ok := result.JSON.(map[string]any)
	if !ok {
		t.Fatal("expected map[string]any")
	}
	if data["name"] != "Nova" {
		t.Errorf("name = %v", data["name"])
	}
}

func TestParseResponse_JSON_WithCodeFence(t *testing.T) {
	raw := "```json\n{\"key\": \"value\"}\n```"
	result, err := ParseResponse(raw, FormatJSON)
	if err != nil {
		t.Fatal(err)
	}
	data, ok := result.JSON.(map[string]any)
	if !ok {
		t.Fatal("expected map[string]any")
	}
	if data["key"] != "value" {
		t.Errorf("key = %v", data["key"])
	}
}

func TestParseResponse_JSON_WithPreamble(t *testing.T) {
	raw := "Here is the data:\n{\"result\": true}"
	result, err := ParseResponse(raw, FormatJSON)
	if err != nil {
		t.Fatal(err)
	}
	data, ok := result.JSON.(map[string]any)
	if !ok {
		t.Fatal("expected map[string]any")
	}
	if data["result"] != true {
		t.Errorf("result = %v", data["result"])
	}
}

func TestParseResponse_JSON_Invalid(t *testing.T) {
	_, err := ParseResponse("not json", FormatJSON)
	if err == nil {
		t.Error("expected error for invalid JSON")
	}
}

func TestParseResponse_JSONArray(t *testing.T) {
	result, err := ParseResponse(`["one", "two", "three"]`, FormatJSONArray)
	if err != nil {
		t.Fatal(err)
	}
	arr, ok := result.JSON.([]any)
	if !ok {
		t.Fatal("expected []any")
	}
	if len(arr) != 3 {
		t.Errorf("expected 3 items, got %d", len(arr))
	}
}

func TestParseResponse_JSONArray_Invalid(t *testing.T) {
	_, err := ParseResponse(`{"not": "array"}`, FormatJSONArray)
	if err == nil {
		t.Error("expected error for non-array JSON")
	}
}

func TestParseResponse_List(t *testing.T) {
	result, err := ParseResponse("apple\nbanana\ncherry", FormatList)
	if err != nil {
		t.Fatal(err)
	}
	if len(result.List) != 3 {
		t.Fatalf("expected 3 items, got %d", len(result.List))
	}
	if result.List[0] != "apple" || result.List[1] != "banana" || result.List[2] != "cherry" {
		t.Errorf("list = %v", result.List)
	}
}

func TestParseResponse_List_StripsPrefixes(t *testing.T) {
	raw := "- first\n* second\n1. third\n2) fourth"
	result, err := ParseResponse(raw, FormatList)
	if err != nil {
		t.Fatal(err)
	}
	if len(result.List) != 4 {
		t.Fatalf("expected 4 items, got %d: %v", len(result.List), result.List)
	}
	if result.List[0] != "first" {
		t.Errorf("item[0] = %q, want %q", result.List[0], "first")
	}
	if result.List[1] != "second" {
		t.Errorf("item[1] = %q, want %q", result.List[1], "second")
	}
	if result.List[2] != "third" {
		t.Errorf("item[2] = %q, want %q", result.List[2], "third")
	}
}

func TestParseResponse_List_SkipsEmpty(t *testing.T) {
	result, err := ParseResponse("a\n\nb\n  \nc", FormatList)
	if err != nil {
		t.Fatal(err)
	}
	if len(result.List) != 3 {
		t.Errorf("expected 3 items, got %d: %v", len(result.List), result.List)
	}
}

func TestParseResponse_KeyValue(t *testing.T) {
	raw := "name: Nova\nversion: 0.1.0\nstatus: active"
	result, err := ParseResponse(raw, FormatKeyValue)
	if err != nil {
		t.Fatal(err)
	}
	if result.KeyValues["name"] != "Nova" {
		t.Errorf("name = %q", result.KeyValues["name"])
	}
	if result.KeyValues["version"] != "0.1.0" {
		t.Errorf("version = %q", result.KeyValues["version"])
	}
	if result.KeyValues["status"] != "active" {
		t.Errorf("status = %q", result.KeyValues["status"])
	}
}

func TestParseResponse_KeyValue_SkipsInvalid(t *testing.T) {
	raw := "valid: yes\nno-colon-here\n: empty key"
	result, err := ParseResponse(raw, FormatKeyValue)
	if err != nil {
		t.Fatal(err)
	}
	if len(result.KeyValues) != 1 {
		t.Errorf("expected 1 key-value, got %d: %v", len(result.KeyValues), result.KeyValues)
	}
}

func TestParseResponse_Bool_Yes(t *testing.T) {
	for _, input := range []string{"yes", "Yes", "YES", "true", "True", "y", "Y"} {
		result, err := ParseResponse(input, FormatBool)
		if err != nil {
			t.Fatal(err)
		}
		if !result.Bool {
			t.Errorf("%q should parse as true", input)
		}
	}
}

func TestParseResponse_Bool_No(t *testing.T) {
	for _, input := range []string{"no", "No", "false", "maybe", "n"} {
		result, err := ParseResponse(input, FormatBool)
		if err != nil {
			t.Fatal(err)
		}
		if result.Bool {
			t.Errorf("%q should parse as false", input)
		}
	}
}

func TestParseResponse_UnknownFormat(t *testing.T) {
	_, err := ParseResponse("test", OutputFormat(99))
	if err == nil {
		t.Error("expected error for unknown format")
	}
}

// --- extractJSON ---

func TestExtractJSON_CleanObject(t *testing.T) {
	result := extractJSON(`{"key": "value"}`)
	if result != `{"key": "value"}` {
		t.Errorf("got %q", result)
	}
}

func TestExtractJSON_CleanArray(t *testing.T) {
	result := extractJSON(`["a", "b"]`)
	if result != `["a", "b"]` {
		t.Errorf("got %q", result)
	}
}

func TestExtractJSON_WithPreamble(t *testing.T) {
	result := extractJSON(`Here is the result: {"data": 42}`)
	if result != `{"data": 42}` {
		t.Errorf("got %q", result)
	}
}

func TestExtractJSON_CodeFence(t *testing.T) {
	result := extractJSON("```json\n{\"x\": 1}\n```")
	if result != `{"x": 1}` {
		t.Errorf("got %q", result)
	}
}

func TestExtractJSON_NestedBraces(t *testing.T) {
	input := `{"outer": {"inner": {"deep": true}}}`
	result := extractJSON(input)
	if result != input {
		t.Errorf("got %q", result)
	}
}

func TestExtractJSON_ArrayBeforeObject(t *testing.T) {
	result := extractJSON(`Some text [1, 2, 3] more text`)
	if result != `[1, 2, 3]` {
		t.Errorf("got %q", result)
	}
}

func TestExtractJSON_NoJSON(t *testing.T) {
	result := extractJSON("no json here")
	if result != "no json here" {
		t.Errorf("should return as-is, got %q", result)
	}
}

func TestExtractJSON_UnmatchedBrace(t *testing.T) {
	result := extractJSON(`{"unclosed": true`)
	// Should return from { to end
	if result != `{"unclosed": true` {
		t.Errorf("got %q", result)
	}
}

// --- parseList ---

func TestParseList_Basic(t *testing.T) {
	list := parseList("one\ntwo\nthree")
	if len(list) != 3 {
		t.Errorf("expected 3, got %d", len(list))
	}
}

func TestParseList_Empty(t *testing.T) {
	list := parseList("")
	if len(list) != 0 {
		t.Errorf("expected empty, got %d", len(list))
	}
}

// --- parseKeyValues ---

func TestParseKeyValues_Basic(t *testing.T) {
	kv := parseKeyValues("a: 1\nb: 2")
	if kv["a"] != "1" || kv["b"] != "2" {
		t.Errorf("kv = %v", kv)
	}
}

func TestParseKeyValues_Empty(t *testing.T) {
	kv := parseKeyValues("")
	if len(kv) != 0 {
		t.Errorf("expected empty, got %d", len(kv))
	}
}

// --- Agent Integration ---

func TestRunStructured_JSON(t *testing.T) {
	provider := &mockProvider{
		responses: []*ChatResponse{
			{Content: `{"answer": 42}`},
		},
	}

	a := New(Config{Provider: provider})
	result, err := a.RunStructured(t.Context(), "What is the answer?", OutputSchema{
		Format: FormatJSON,
	})
	if err != nil {
		t.Fatal(err)
	}

	data, ok := result.JSON.(map[string]any)
	if !ok {
		t.Fatal("expected map[string]any")
	}
	if data["answer"] != float64(42) {
		t.Errorf("answer = %v", data["answer"])
	}
	if result.Usage == nil {
		t.Error("usage should be set")
	}
}

func TestRunStructured_ParseError(t *testing.T) {
	provider := &mockProvider{
		responses: []*ChatResponse{
			{Content: "not valid json"},
		},
	}

	a := New(Config{Provider: provider})
	result, err := a.RunStructured(t.Context(), "test", OutputSchema{
		Format: FormatJSON,
	})
	if err == nil {
		t.Error("expected parse error")
	}
	// Should still return partial result
	if result == nil {
		t.Error("should return partial result even on parse error")
	}
	if result.Raw != "not valid json" {
		t.Error("raw should be preserved even on parse error")
	}
}

func TestRunStructured_ProviderError(t *testing.T) {
	provider := &errorProvider{err: errTest}

	a := New(Config{Provider: provider})
	_, err := a.RunStructured(t.Context(), "test", OutputSchema{Format: FormatJSON})
	if err == nil {
		t.Error("expected provider error")
	}
}

func TestRunJSON(t *testing.T) {
	provider := &mockProvider{
		responses: []*ChatResponse{
			{Content: `{"name": "Nova"}`},
		},
	}

	a := New(Config{Provider: provider})
	result, err := a.RunJSON(t.Context(), "Who are you?",
		SchemaField{Name: "name", Type: "string", Required: true},
	)
	if err != nil {
		t.Fatal(err)
	}

	data, ok := result.JSON.(map[string]any)
	if !ok {
		t.Fatal("expected map")
	}
	if data["name"] != "Nova" {
		t.Errorf("name = %v", data["name"])
	}
}

func TestRunBool(t *testing.T) {
	provider := &mockProvider{
		responses: []*ChatResponse{
			{Content: "yes"},
		},
	}

	a := New(Config{Provider: provider})
	result, err := a.RunBool(t.Context(), "Is the sky blue?")
	if err != nil {
		t.Fatal(err)
	}
	if !result {
		t.Error("expected true")
	}
}

func TestRunBool_Error(t *testing.T) {
	provider := &errorProvider{err: errTest}
	a := New(Config{Provider: provider})
	_, err := a.RunBool(t.Context(), "test")
	if err == nil {
		t.Error("expected error")
	}
}

func TestRunList(t *testing.T) {
	provider := &mockProvider{
		responses: []*ChatResponse{
			{Content: "alpha\nbeta\ngamma"},
		},
	}

	a := New(Config{Provider: provider})
	result, err := a.RunList(t.Context(), "List Greek letters")
	if err != nil {
		t.Fatal(err)
	}
	if len(result) != 3 {
		t.Fatalf("expected 3 items, got %d", len(result))
	}
}

func TestRunList_Error(t *testing.T) {
	provider := &errorProvider{err: errTest}
	a := New(Config{Provider: provider})
	_, err := a.RunList(t.Context(), "test")
	if err == nil {
		t.Error("expected error")
	}
}

// --- Helper ---

func containsStr(s, substr string) bool {
	return len(s) >= len(substr) && searchStr(s, substr)
}

func searchStr(s, sub string) bool {
	for i := 0; i <= len(s)-len(sub); i++ {
		if s[i:i+len(sub)] == sub {
			return true
		}
	}
	return false
}
