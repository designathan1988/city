"""The research table (log/trace-rules.md) as data: each construct of each grammar and the rules that handle it.

Grouped by rule set; `tests/test_rules.py` keeps it equal to the log and to the installed grammars.
"""
from __future__ import annotations

RULE_IDS = frozenset({"DEF", "LOOK", "VALUE", "FLOW", "CALL", "BRANCH", "JUMP", "NAME", "SIGNAL", "BIND", "ENTRY",
                      "TYPE", "NONE", "EXPR"})


def _table(groups: list[tuple[tuple[str, ...], str]]) -> dict[str, tuple[str, ...]]:
    return {kind: ids for ids, kinds in groups for kind in kinds.split()}

GD_RULES = _table([
    (('DEF',),
     "source class_definition class_body static_keyword enum_definition enumerator_list enumerator name"),
    (('DEF', 'LOOK'),
     "class_name_statement extends_statement"),
    (('DEF', 'ENTRY'),
     "function_definition constructor_definition"),
    (('DEF', 'FLOW'),
     "parameters typed_parameter default_parameter typed_default_parameter variadic_parameter "
     "variable_statement onready_variable_statement export_variable_statement const_statement"),
    (('BRANCH',),
     "body if_statement elif_clause else_clause while_statement"),
    (('DEF', 'CALL'),
     "setget setter getter get_body set_body"),
    (('DEF', 'SIGNAL'),
     "signal_statement"),
    (('DEF', 'VALUE'),
     "lambda"),
    (('TYPE', 'FLOW'),
     "type inferred_type"),
    (('DEF', 'NAME'),
     "annotation annotations"),
    (('NAME',),
     "remote_keyword string_name"),
    (('VALUE',),
     "expression_statement"),
    (('FLOW',),
     "assignment augmented_assignment"),
    (('BRANCH', 'FLOW'),
     "match_statement match_body pattern_section pattern_guard pattern_binding pattern_open_ending "
     "for_statement"),
    (('JUMP', 'FLOW'),
     "return_statement"),
    (('JUMP', 'NONE'),
     "break_statement continue_statement pass_statement breakpoint_statement"),
    (('JUMP', 'EXPR'),
     "await_expression"),
    (('LOOK', 'EXPR'),
     "identifier"),
    (('EXPR',),
     "attribute attribute_subscript subscript subscript_arguments binary_operator unary_operator "
     "parenthesized_expression get_node node_path string integer float true false null escape_sequence"),
    (('CALL', 'EXPR'),
     "attribute_call"),
    (('CALL', 'NAME', 'EXPR'),
     "call"),
    (('CALL',),
     "base_call arguments"),
    (('EXPR', 'BRANCH'),
     "conditional_expression"),
    (('EXPR', 'FLOW'),
     "array dictionary pair"),
    (('NONE',),
     "line_continuation comment region_start region_end region_label"),
])

CPP_RULES = _table([
    (('DEF', 'LOOK'),
     "translation_unit namespace_definition namespace_identifier nested_namespace_specifier "
     "namespace_alias_definition alias_declaration type_definition"),
    (('LOOK',),
     "preproc_include system_lib_string using_declaration"),
    (('DEF',),
     "preproc_def preproc_function_def preproc_params preproc_arg preproc_call preproc_directive "
     "enum_specifier enumerator_list enumerator default_method_clause delete_method_clause pure_virtual_clause "
     "virtual_specifier explicit_function_specifier storage_class_specifier type_qualifier ref_qualifier "
     "noexcept throw_specifier alignas_qualifier operator_name destructor_name operator_cast"),
    (('BRANCH',),
     "preproc_if preproc_ifdef preproc_elif preproc_elifdef preproc_else preproc_defined if_statement "
     "condition_clause else_clause init_statement switch_statement case_statement while_statement do_statement"),
    (('DEF', 'ENTRY'),
     "linkage_specification"),
    (('DEF', 'BRANCH'),
     "declaration_list field_declaration_list compound_statement"),
    (('DEF', 'LOOK', 'CALL'),
     "class_specifier struct_specifier union_specifier base_class_clause access_specifier"),
    (('DEF', 'FLOW'),
     "function_definition function_declarator trailing_return_type field_initializer_list field_initializer "
     "declaration field_declaration init_declarator parameter_list parameter_declaration "
     "optional_parameter_declaration variadic_parameter_declaration pointer_declarator reference_declarator "
     "array_declarator parenthesized_declarator attributed_declarator structured_binding_declarator "
     "variadic_declarator new_declarator bitfield_clause"),
    (('TYPE',),
     "abstract_array_declarator abstract_function_declarator abstract_parenthesized_declarator "
     "abstract_pointer_declarator abstract_reference_declarator pointer_type_declarator concept_definition "
     "requires_clause requires_expression requirement_seq simple_requirement compound_requirement "
     "type_requirement constraint_conjunction constraint_disjunction"),
    (('DEF', 'VALUE'),
     "template_declaration template_parameter_list type_parameter_declaration "
     "optional_type_parameter_declaration variadic_type_parameter_declaration "
     "template_template_parameter_declaration template_instantiation"),
    (('NONE',),
     "friend_declaration static_assert_declaration attribute_declaration attribute attribute_specifier "
     "ms_based_modifier ms_call_modifier ms_declspec_modifier ms_pointer_modifier ms_restrict_modifier "
     "ms_signed_ptr_modifier ms_unaligned_ptr_modifier ms_unsigned_ptr_modifier gnu_asm_expression "
     "gnu_asm_qualifier gnu_asm_output_operand_list gnu_asm_output_operand gnu_asm_input_operand_list "
     "gnu_asm_input_operand gnu_asm_clobber_list gnu_asm_goto_list comment"),
    (('TYPE', 'FLOW'),
     "primitive_type sized_type_specifier type_identifier template_type template_argument_list type_descriptor "
     "dependent_type dependent_name placeholder_type_specifier auto decltype"),
    (('VALUE',),
     "expression_statement"),
    (('BRANCH', 'FLOW'),
     "for_statement for_range_loop"),
    (('JUMP', 'FLOW'),
     "return_statement co_return_statement"),
    (('JUMP',),
     "break_statement continue_statement goto_statement labeled_statement statement_identifier"),
    (('BRANCH', 'JUMP'),
     "try_statement catch_clause throw_statement seh_try_statement seh_except_clause seh_finally_clause "
     "seh_leave_statement"),
    (('JUMP', 'EXPR'),
     "co_yield_statement co_await_expression"),
    (('VALUE', 'NONE'),
     "attributed_statement"),
    (('LOOK', 'EXPR'),
     "identifier field_identifier qualified_identifier"),
    (('EXPR',),
     "this field_expression subscript_expression subscript_argument_list pointer_expression cast_expression "
     "compound_literal_expression sizeof_expression alignof_expression offsetof_expression generic_expression "
     "extension_expression fold_expression parameter_pack_expansion parenthesized_expression number_literal "
     "char_literal character string_literal raw_string_literal concatenated_string string_content "
     "raw_string_content raw_string_delimiter escape_sequence user_defined_literal literal_suffix true false "
     "null"),
    (('CALL', 'EXPR'),
     "call_expression argument_list template_function template_method new_expression delete_expression"),
    (('EXPR', 'FLOW'),
     "unary_expression binary_expression update_expression assignment_expression comma_expression "
     "initializer_list initializer_pair field_designator subscript_designator subscript_range_designator"),
    (('EXPR', 'BRANCH'),
     "conditional_expression"),
    (('DEF', 'EXPR'),
     "lambda_expression lambda_capture_specifier lambda_default_capture lambda_capture_initializer"),
])
