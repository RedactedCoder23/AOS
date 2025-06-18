# AOS — ./tests/test_merge_hypothesis.py
# (c) 2025 RedactedCoder23
# Brief: Source file.
from hypothesis import given, strategies as st
from src.branch.merge import three_way_merge


@given(st.text(), st.text(), st.text())
def test_output_types(base, ours, theirs):
    merged, conflicts = three_way_merge(base, ours, theirs)
    assert isinstance(merged, str)
    assert isinstance(conflicts, list)


@given(st.text())
def test_no_conflict_identical(text):
    _, conflicts = three_way_merge(text, text, text)
    assert conflicts == []


@given(st.text(), st.text())
def test_merge_symmetry(base, other):
    a, _ = three_way_merge(base, other, other)
    b, _ = three_way_merge(base, base, other)
    assert a == b


@given(st.text(), st.text(), st.text())
def test_conflict_marker(base, ours, theirs):
    merged, conflicts = three_way_merge(base, ours, theirs)
    if conflicts:
        assert "<<<<<<" in merged


@given(st.text())
def test_handles_empty_inputs(text):
    merged, conflicts = three_way_merge("", text, "")
    assert isinstance(merged, str)


@given(st.text(), st.text())
def test_commutativity(base, ours):
    m1, _ = three_way_merge(base, ours, base)
    m2, _ = three_way_merge(base, base, ours)
    assert isinstance(m1, str) and isinstance(m2, str)


@given(st.text())
def test_no_exception(text):
    three_way_merge(text, text[::-1], text)


@given(st.text())
def test_returns_string_even_with_unicode(text):
    merged, _ = three_way_merge(text, text.upper(), text.lower())
    assert isinstance(merged, str)
