/*
 * Assignment: Operating Systems
 * Practise: Third Practise
 * Deadline: 17th of December
 *
 * The goal is to simulate memory management with
 * dynamic partitions. Furthermore we implemented
 * native graphics to visualize the algorithm by
 * using TGUI as a wrapper of the SFML GUI library.
 *
 * Developed by:
 *  - Ignacio Encinas Rubio [1]
 *  - Ernesto Martínez García [1]
 *
 *  [1] Computer Science, University of Alicante Polytechnic School
 *
 * Dependencies:
 *  - sfml https://www.sfml-dev.org/
 *  - tgui https://tgui.eu/
 */

#include "MemoryManager.h"
#include <iostream>
#include <cstdlib>
#include <TGUI/TGUI.hpp> /* TGUI include SFML */

#define memoryWidth(wx) wx/5
#define memoryHeight(wy) 3*wy/5
#define upperMargin(wy) 0.10f * wy
#define textSize(wx) wx * 0.012
#define bigOutline(wx) wx * 0.0035
#define smallOutline(wx) wx * 0.0005

float memoryScaling = 0.0005f;

#define random (unsigned)rand()

float wx = 0;
float wy = 0;

sf::RectangleShape 
createRectangle(const Process &p, sf::Vector2f offset = {memoryWidth(wx), upperMargin(wy)}) {
    int auxRand = 0;
    /* We use the name of the processes to generate a color */
    for(unsigned i = 0; i < strlen(p.name); i++) {
        auxRand += p.name[i];
        auxRand = auxRand << 1;
    }

    srand(auxRand);
    sf::RectangleShape newRectangle(sf::Vector2f(memoryWidth(wx), (float)(p.memorySize * memoryHeight(wy) * memoryScaling)));
    newRectangle.setPosition(offset.x, offset.y + (float)(p.startingAddress * memoryHeight(wy) * memoryScaling));
    newRectangle.setFillColor(sf::Color(random + 4 * auxRand));
    newRectangle.setOutlineColor(sf::Color::White);
    newRectangle.setOutlineThickness(smallOutline(wx));

    return newRectangle;
}

/* This sucks, I'm sorry. GUI work is annoying =( */
void 
drawMemory(const MemoryManager &m, sf::RenderWindow &window) {
    // Processes from the memory manager
    for(auto itr = m.memory[m.getTime()].begin(); itr != m.memory[m.getTime()].end(); itr++) {
        if((*itr).running) {
            sf::RectangleShape rect = createRectangle(*itr); 
            window.draw(rect);
        }
    }

    int acumulatedOffset = 0;
    for(auto itr = m.waiting[m.getTime()].begin(); itr != m.waiting[m.getTime()].end(); itr++) {
        /* We stay inside the base rectangle. The '4/3' is totally arbitrary */
        if(acumulatedOffset + (*itr).memorySize > m.maxMemory * 4 / 3)
            break;
        
        sf::RectangleShape rect = createRectangle(*itr, {2.25f * memoryWidth(wx), upperMargin(wy)}); 
        rect.setPosition(2.25 * memoryWidth(wx), upperMargin(wy) + (float)acumulatedOffset * memoryHeight(wy) * memoryScaling);
        acumulatedOffset += (*itr).memorySize;
        window.draw(rect);

    }

    acumulatedOffset = 0;
    for(auto itr = m.toArrive[m.getTime()].begin(); itr != m.toArrive[m.getTime()].end(); itr++) {
        /* We stay inside the base rectangle. The '4/3' is totally arbitrary */
        if(acumulatedOffset + (*itr).memorySize > m.maxMemory * 4 / 3)
            break;

        sf::RectangleShape rect = createRectangle(*itr, {3.5f * memoryWidth(wx), upperMargin(wy)}); 
        rect.setPosition(3.5 * memoryWidth(wx), upperMargin(wy) + (float)acumulatedOffset * memoryHeight(wy) * memoryScaling);
        acumulatedOffset += (*itr).memorySize;
        window.draw(rect);

    }
}

void 
loadTGUILabels(tgui::Gui&, tgui::Theme&, tgui::Label::Ptr&, tgui::Label::Ptr&, 
        tgui::Label::Ptr&, tgui::Label::Ptr&, tgui::Label::Ptr&);


void createBaseRectangles(sf::RectangleShape&, sf::RectangleShape&, sf::RectangleShape&);

int 
main(int argc, char *argv[]) {
    if(argc < 2){
        std::cout << "Usage: " << argv[0] << " <PROC_FILE>" << std::endl;
        return -1;
    }

    srand(133);

    /* Default size and default algorithm */
    MemoryManager m(2000, Alloc::Bf);
    memoryScaling = 1.0f / m.maxMemory;

    if(!m.readFile(argv[1])){
        std::cout << "[!] Error: File not found" << endl;
        return -1;
    }

    /*
     * Create the window. Change the resolution/mode as you wish 
     * Disclaimer: The text gets weird after rescaling, so dont get annoyed
     * by that. Just create the window and let it be.
     */
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "(N)VIM GVNG", sf::Style::Fullscreen);
    window.setFramerateLimit(60);
    wx = window.getSize().x;
    wy = window.getSize().y;

    // Base rectangle of memory, waiting, toArrive ...
    sf::RectangleShape memoryBase, waitingBase, toArriveBase;
    createBaseRectangles(memoryBase, waitingBase, toArriveBase);

    /* Tgui theme for the GUI */
    tgui::Gui gui(window);
    tgui::Theme theme{"tgui/themes/Black.txt"};

    tgui::Label::Ptr sliderLabel, memoryLabel, waitingLabel, toArriveLabel, stepsLabel;
    
    loadTGUILabels(gui, theme, sliderLabel, memoryLabel, waitingLabel, toArriveLabel, stepsLabel);

    auto slider = tgui::Slider::create();
    slider->setMaximum(20);
    slider->setRenderer(theme.getRenderer("Slider"));
    slider->setPosition(wx * 0.02, wy * 0.15);
    slider->setSize(wx * 0.15, wx * 0.02);
    slider->setValue(0);
    gui.add(slider);

    auto stepsSlider = tgui::Slider::create();
    stepsSlider->setMaximum(20);
    stepsSlider->setMinimum(0);
    stepsSlider->setRenderer(theme.getRenderer("Slider"));
    stepsSlider->setPosition(wx * 0.02, wy * 0.5);
    stepsSlider->setSize(wx * 0.15, wx * 0.02);
    stepsSlider->setValue(10);
    gui.add(stepsSlider);

    auto algoLabel = tgui::Button::create();
    algoLabel->setRenderer(theme.getRenderer("Button"));
    algoLabel->setPosition(wx * 0.02, wy * 0.60);
    algoLabel->setSize(wx * 0.15, wy * 0.06);
    algoLabel->setText(std::string(m.getAlgorithm()));
    algoLabel->setTextSize(textSize(wx));
    algoLabel->connect("pressed", [&](){ m.setAlgorithm(m.allocationAlgorithm == Alloc::Ff ? Alloc::Bf : Alloc::Ff); });
    gui.add(algoLabel);


    auto close = tgui::Button::create();
    close->setRenderer(theme.getRenderer("Button"));
    close->setSize(wx * 0.2, wy * 0.05);
    close->setText("Exit");
    close->setTextSize(textSize(wx));
    close->setPosition(wx * 0.07, wy * 0.8);
    close->connect("pressed", [&](){ window.close(); });
    gui.add(close);

    /* Run/Stop for automatic execution */
    sf::Clock clock = sf::Clock();
    float lastTick = 0;

    bool autoRun = false;
    auto button = tgui::Button::create();
    button->setRenderer(theme.getRenderer("Button"));
    button->setPosition(wx * 0.02, wy * 0.30);
    button->setSize(wx * 0.15, wx * 0.05);
    button->setText("Run");
    button->setTextSize(textSize(wx));
    button->connect("pressed", [&](){ clock.restart(); lastTick = 0; autoRun = !autoRun; button->setText(autoRun ? "Stop" : "Run"); });
    gui.add(button);

    int firstIteration = 0;
    float stepsPerSecond = stepsSlider->getValue() / 20;

    while (window.isOpen()) {

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            gui.handleEvent(event);
        }

        // clear the window with black color
        window.clear(sf::Color::Black);

        window.draw(memoryBase);
        window.draw(waitingBase);
        window.draw(toArriveBase);

        if(firstIteration == 0) {
            firstIteration++;
        } else if(firstIteration != 0) {
            stepsPerSecond = stepsSlider->getValue() / 10;

            stepsLabel->setText("Time speed x" + std::to_string(stepsPerSecond).substr(0, 3));

            /* Slider gets bigger if necessary */
            if(slider->getMaximum() - slider->getValue() == 5 && (m.memory[m.getTime()].size() != 2 ||
                        m.waiting[m.getTime()].size() != 0 || m.toArrive[m.getTime()].size() != 0))
                slider->setMaximum(slider->getMaximum() + 5);

            /* Updating time widgets */
            if(autoRun && clock.getElapsedTime().asSeconds() - lastTick >= (1/stepsPerSecond) && slider->getValue() != slider->getMaximum()) {
                lastTick = clock.getElapsedTime().asSeconds();
                slider->setValue(slider->getValue() + 1);
            }

            /* Update memory */
            m.guiTime(slider->getValue());

            sf::String time(std::to_string(m.getTime()));
            sliderLabel->setText("Current time: " + time);

            /* We dont count 'head' and 'tail' artificially added */
            memoryLabel->setText("Memory\n" + std::to_string(m.memory[m.getTime()].size() - 2));
            waitingLabel->setText("Waiting\n" + std::to_string(m.waiting[m.getTime()].size()));
            toArriveLabel->setText("To arrive\n" + std::to_string(m.toArrive[m.getTime()].size()));

            algoLabel->setText(std::string(m.getAlgorithm()));
        }

        /* Draw memory, waiting y toArrive */
        drawMemory(m, window);

        gui.draw();
        window.display();
    }
    return 0;
}

void
createBaseRectangles(sf::RectangleShape &memoryBase, sf::RectangleShape &waitingBase, sf::RectangleShape &toArriveBase) {

    memoryBase = sf::RectangleShape(sf::Vector2f(memoryWidth(wx), memoryHeight(wy)));
    memoryBase.setPosition(memoryWidth(wx), upperMargin(wy));
    memoryBase.setFillColor(sf::Color::Black);
    memoryBase.setOutlineThickness(bigOutline(wx));
    memoryBase.setOutlineColor(sf::Color::White);

    waitingBase = sf::RectangleShape(sf::Vector2f(memoryWidth(wx), memoryHeight(wy) / 3 * 4));
    waitingBase.setPosition(2.25 * memoryWidth(wx), upperMargin(wy));
    waitingBase.setFillColor(sf::Color::Black);
    waitingBase.setOutlineThickness(bigOutline(wx));
    waitingBase.setOutlineColor(sf::Color::White);

    toArriveBase = sf::RectangleShape(sf::Vector2f(memoryWidth(wx), memoryHeight(wy) / 3 * 4));
    toArriveBase.setPosition(3.5 * memoryWidth(wx), upperMargin(wy));
    toArriveBase.setFillColor(sf::Color::Black);
    toArriveBase.setOutlineThickness(bigOutline(wx));
    toArriveBase.setOutlineColor(sf::Color::White);
}

/* God forgive me */
void 
loadTGUILabels(tgui::Gui &gui, tgui::Theme &theme, tgui::Label::Ptr &sliderLabel, 
        tgui::Label::Ptr &memoryLabel, tgui::Label::Ptr &waitingLabel, tgui::Label::Ptr &toArriveLabel,
        tgui::Label::Ptr &stepsLabel) {

    sliderLabel = tgui::Label::create();
    sliderLabel->setRenderer(theme.getRenderer("Label"));
    sliderLabel->setText("Current time");
    sliderLabel->setPosition(wx * 0.0025, wy * 0.05);
    sliderLabel->setTextSize(textSize(wx));
    gui.add(sliderLabel);

    memoryLabel = tgui::Label::create();
    memoryLabel->setRenderer(theme.getRenderer("Label"));
    memoryLabel->setText("");
    memoryLabel->setPosition(1.35 * memoryWidth(wx), 10);
    memoryLabel->setTextSize(textSize(wx));
    gui.add(memoryLabel);

    waitingLabel = tgui::Label::create();
    waitingLabel->setRenderer(theme.getRenderer("Label"));
    waitingLabel->setText("");
    waitingLabel->setPosition(2.65 * memoryWidth(wx), 10);
    waitingLabel->setTextSize(textSize(wx));
    gui.add(waitingLabel);

    toArriveLabel = tgui::Label::create();
    toArriveLabel->setRenderer(theme.getRenderer("Label"));
    toArriveLabel->setText("");
    toArriveLabel->setPosition(3.85 * memoryWidth(wx), 10);
    toArriveLabel->setTextSize(textSize(wx));
    gui.add(toArriveLabel);

    stepsLabel = tgui::Label::create();
    stepsLabel->setRenderer(theme.getRenderer("Label"));
    stepsLabel->setText("Time speed x1");
    stepsLabel->setPosition(wx * 0.0025, wy * 0.45);
    stepsLabel->setTextSize(textSize(wx));
    gui.add(stepsLabel);
}
