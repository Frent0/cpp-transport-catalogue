#include "json_builder.h"

namespace json {

    using namespace std::literals;

    Builder::DictItemContext Builder::StartDict() {
        if (step_stack_.empty()) throw std::logic_error("Must start with builder"s);
        if (root_) throw std::logic_error("Build finished"s);
        ++dicts_open_;
        keys_.resize(dicts_open_);
        all_dicts_.resize(dicts_open_);
        step_stack_.push_back(Step::DICT);
        return DictItemContext(*this);
    }

    Builder& Builder::EndDict() {
        if (step_stack_.back() == Step::DICT) {
            if (dicts_open_ > 0) {
                Dict dict;
                for (auto& [key, val] : all_dicts_[dicts_open_ - 1]) {
                    dict[key] = std::move(val);
                }
                step_stack_.pop_back();
                --dicts_open_;
                keys_.resize(dicts_open_);
                all_dicts_.resize(dicts_open_);
                Node node_dict(std::move(dict));
                AddNode(std::move(node_dict));
            }
            else throw std::logic_error("No dicts open"s);
        }
        else throw std::logic_error("Cannot close dict"s);
        return *this;
    }

    Builder::ArrayContext Builder::StartArray() {
        if (step_stack_.empty()) throw std::logic_error("Must start with builder"s);
        if (root_) throw std::logic_error("Build finished"s);
        ++arrays_open_;
        all_arrays_.resize(arrays_open_);
        step_stack_.push_back(Step::ARR);
        return *this;
    }

    Builder& Builder::EndArray() {
        if (step_stack_.back() == Step::ARR) {
            if (arrays_open_ > 0) {
                Array arr;
                for (auto& elem : all_arrays_[arrays_open_ - 1]) {
                    arr.push_back(std::move(elem));
                }
                step_stack_.pop_back();
                --arrays_open_;
                all_arrays_.resize(arrays_open_);
                Node node_arr(std::move(arr));
                AddNode(std::move(node_arr));
            }
            else throw std::logic_error("No arrays open"s);
        }
        else throw std::logic_error("Cannot close array"s);
        return *this;
    }

    Builder::DictValueContext Builder::Key(const std::string& key) {
        if (step_stack_.back() == Step::DICT) {
            if (dicts_open_ > 0) {
                if (!keys_[dicts_open_ - 1]) {
                    keys_[dicts_open_ - 1] = key;
                }
                else throw std::logic_error("Key already set"s);
            }
            else throw std::logic_error("No dicts open"s);
        }
        else throw std::logic_error("Must start a dict"s);
        return *this;
    }

    Builder& Builder::Value(const Node::Value& val) {
        if (step_stack_.empty()) throw std::logic_error("Must start with builder"s);
        if (root_) throw std::logic_error("Build finished"s);

        Node node(val);
        AddNode(std::move(node));

        return *this;
    }



    void Builder::AddNode(Node&& node) {
        if (step_stack_.back() == Step::ARR) {
            if (arrays_open_ > 0) {
                all_arrays_[arrays_open_ - 1].push_back(std::move(node));
            }
            else throw std::logic_error("No arrays open"s);
        }
        else if (step_stack_.back() == Step::DICT) {
            if (dicts_open_ > 0) {
                if (keys_[dicts_open_ - 1]) {
                    all_dicts_[dicts_open_ - 1].push_back({ keys_[dicts_open_ - 1].value(), std::move(node) });
                    keys_[dicts_open_ - 1].reset();
                }
                else throw std::logic_error("No key"s);
            }
            else throw std::logic_error("No dicts open"s);
        }
        else {
            root_ = std::move(node);
        }
    }

    Node Builder::Build() const {
        if (root_) {
            return root_.value();
        }
        else throw std::logic_error("Must finish build"s);
    }

    Builder::DictValueContext Builder::DictItemContext::Key(const std::string& key) {
        return builder_.Key(key);
    }

    Builder& Builder::DictItemContext::EndDict() {
        return builder_.EndDict();
    }

    Builder::DictItemContext Builder::DictValueContext::Value(const Node::Value& val) {
        return builder_.Value(val);
    }

    Builder::DictItemContext Builder::DictValueContext::StartDict() {
        return builder_.StartDict();
    }

    Builder::ArrayContext Builder::DictValueContext::StartArray() {
        return builder_.StartArray();
    }

    Builder::ArrayContext Builder::ArrayContext::Value(const Node::Value& val) {
        return builder_.Value(val);
    }

    Builder::DictItemContext Builder::ArrayContext::StartDict() {
        return builder_.StartDict();
    }

    Builder::ArrayContext Builder::ArrayContext::StartArray() {
        return builder_.StartArray();
    }

    Builder& Builder::ArrayContext::EndArray() {
        return builder_.EndArray();
    }

}